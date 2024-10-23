//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Maths.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

#include <algorithm>
#include <corecrt_io.h>
#include <execution>

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);

	m_Pixels.resize(m_Width * m_Height);
	std::iota(m_Pixels.begin(), m_Pixels.end(), 0);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera{ pScene->GetCamera() };
	auto const& materials{ pScene->GetMaterials() };
	auto const& lights { pScene->GetLights() };

	float const aspectRatio{ m_Width / static_cast<float>(m_Height) };
	float const fov{ tan(camera.fovAngle * TO_RADIANS/2) }; //TODO -calculate only when updating

	Matrix const cameraToWorld{ camera.CalculateCameraToWorld() };
	
	std::for_each(std::execution::par_unseq, m_Pixels.begin(), m_Pixels.end(), [&](int const idx)
	{
		ColorRGB finalColor{ };

		int const px{ idx % m_Width };
		int const py{ idx / m_Width };

		for (uint32_t currSample{ 0 }; currSample < m_SampleCount; ++currSample)
		{
			auto const offset{ SampleRay(currSample) };

			float const x{ ((2 * (px + .5f + offset.x) / static_cast<float>(m_Width) - 1) * aspectRatio * fov) };
			float const y{ ((1 - 2 * (py + .5f + offset.y) / static_cast<float>(m_Height)) * fov) };

			Vector3 const dirViewSpace{ x , y, 1.f };
			Vector3 const dirWorldSpace{ (cameraToWorld.TransformVector(dirViewSpace)).Normalized() };

			Ray const viewRay{ cameraToWorld.GetTranslation() , dirWorldSpace };

			HitRecord closestHit{ };
			pScene->GetClosestHit(viewRay, closestHit);

			if (closestHit.didHit)
			{
				for (auto const& light : lights)
				{
					auto dirToLight{ GetDirectionToLight(light, closestHit.origin) };
					auto const distance{ dirToLight.Normalize() };

					Ray const shadowRay{ closestHit.origin, dirToLight, 0.001f, distance };

					if (m_ShadowsEnabled && pScene->DoesHit(shadowRay))
					{
						continue;
					}

					switch (m_CurrLightMode)
					{
					case LightMode::ObservedArea:
					{
						auto const observedArea{ GetObservedArea(light, dirToLight, closestHit.normal) };
						if (observedArea < 0.f)
						{
							continue;
						}
						finalColor += observedArea;

						break;
					}
					case LightMode::Radiance:
					{
						auto const radiance{ GetRadiance(light, closestHit.origin) };
						finalColor += radiance;

						break;
					}
					case LightMode::BRDF:
					{
						auto const observedArea{ GetObservedArea(light, dirToLight, closestHit.normal) };
						if (observedArea < 0.f)
						{
							continue;
						}

						finalColor += materials[closestHit.materialIndex]->Shade(closestHit, dirToLight, -viewRay.direction);

						break;
					}
					case LightMode::Combined:
					{
						auto const observedArea{ GetObservedArea(light, dirToLight, closestHit.normal) };

						if (observedArea < 0.f)
						{
							continue;
						}

						auto const radiance{ GetRadiance(light, closestHit.origin) };
						finalColor += radiance * materials[closestHit.materialIndex]->Shade(closestHit, dirToLight, -viewRay.direction) * observedArea;

						break;
					}
					default:
						break;
					}
				}
			}
		}

		BoxFilter(finalColor);
		finalColor.MaxToOne();

		//Different forms of mapping the final colour
		//ReinhardJolieToneMap(finalColor);
		//ACESAproxToneMap(finalColor);

		m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
			static_cast<uint8_t>(finalColor.r * 255),
			static_cast<uint8_t>(finalColor.g * 255),
			static_cast<uint8_t>(finalColor.b * 255));
			
	});

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

Vector3 dae::Renderer::SampleRay(uint32_t currSample) const noexcept
{
	if (m_SampleCount == 1)
	{
		return {};
	}

	switch(m_CurrSampleMode)
	{
	case SampleMode::RandomSquare:
		return SampleRandomSquare();
	case SampleMode::UniformSquare:
		return SampleUniformSquare(currSample);
	default: 
		return {};
	}
}

Vector3 dae::Renderer::SampleRandomSquare() const noexcept
{
	return {Utils::Random(0.f, 1.f) - .5f, Utils::Random(0.f, 1.f) - .5f, 0.f};
}

Vector3 dae::Renderer::SampleUniformSquare(uint32_t currSample) const noexcept
{

	uint32_t gridSize{ static_cast<uint32_t>(std::sqrt(m_SampleCount)) };

	if (gridSize * gridSize < m_SampleCount)
	{
		++gridSize;
	}

	float const subpixelWidth{ 1.0f / gridSize };
	float const subpixelHeight{ 1.0f / gridSize };

	uint32_t const sampleX{ currSample % gridSize };
	uint32_t const sampleY { currSample / gridSize };

	if (m_SampleCount == 2) //When there are only 2 samples, just do the samples on the center line
	{
		return Vector3{ (currSample * subpixelWidth) + (0.5f * subpixelWidth) - .5f, 0.5f, 0.0f };
	}

	return Vector3
	{
		(sampleX * subpixelWidth) + (0.5f * subpixelWidth) - .5f,
		(sampleY * subpixelHeight) + (0.5f * subpixelHeight) - .5f,
		0.0f 
	};
}

void dae::Renderer::BoxFilter(ColorRGB& c) const noexcept
{
	c /= m_SampleCount;
}
