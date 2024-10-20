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

		for (uint32_t currSample{ 0 }; currSample < m_Samplecount; ++currSample)
		{
			auto const offset{ SampleSquare() };

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
					auto dirToLight{ LightUtils::GetDirectionToLight(light, closestHit.origin) };
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
						auto const observedArea{ LightUtils::GetObservedArea(light, dirToLight, closestHit.normal) };
						if (observedArea < 0.f)
						{
							continue;
						}
						finalColor += observedArea;

						break;
					}
					case LightMode::Radiance:
					{
						auto const radiance{ LightUtils::GetRadiance(light, closestHit.origin) };
						finalColor += radiance;

						break;
					}
					case LightMode::BRDF:
					{
						auto const observedArea{ LightUtils::GetObservedArea(light, dirToLight, closestHit.normal) };
						if (observedArea < 0.f)
						{
							continue;
						}

						finalColor += materials[closestHit.materialIndex]->Shade(closestHit, dirToLight, -viewRay.direction);

						break;
					}
					case LightMode::Combined:
					{
						auto const observedArea{ LightUtils::GetObservedArea(light, dirToLight, closestHit.normal) };

						if (observedArea < 0.f)
						{
							continue;
						}

						auto const radiance{ LightUtils::GetRadiance(light, closestHit.origin) };
						finalColor += radiance * materials[closestHit.materialIndex]->Shade(closestHit, dirToLight, -viewRay.direction) * observedArea;

						break;
					}
					default:
						break;
					}
				}
			}
		}

		finalColor /= m_Samplecount;

		finalColor.MaxToOne();
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

Vector3 dae::Renderer::SampleSquare() const noexcept
{
	if (m_Samplecount > 1)
	{
		return {Utils::Random(0.f, 1.f) - .5f, Utils::Random(0.f, 1.f) - .5f, 0.f};
	}

	return { };
}
