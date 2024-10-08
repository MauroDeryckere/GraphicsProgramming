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
		int const px{ idx % m_Width };
		int const py{ idx / m_Width };

		float const x{ (2 * (px + .5f) / static_cast<float>(m_Width) - 1) * aspectRatio * fov };
		float const y{ (1 - 2 * (py + .5f) / static_cast<float>(m_Height)) * fov };

		Vector3 const dirViewSpace{ x , y, 1.f };
		Vector3 const dirWorldSpace{ (cameraToWorld.TransformVector(dirViewSpace)).Normalized() };

		Ray const viewRay{ cameraToWorld.GetTranslation() , dirWorldSpace };

		HitRecord closestHit{ };
		pScene->GetClosestHit(viewRay, closestHit);

		ColorRGB finalColor{ };

		if (closestHit.didHit)
		{
			for (auto const& light : lights)
			{
				auto dirToLight{ LightUtils::GetDirectionToLight(light, closestHit.origin) };
				auto const distance{ dirToLight.Normalize() };

				Ray const shadowRay{ closestHit.origin, dirToLight, 0.0001f, distance };
				if (pScene->DoesHit(shadowRay) && m_ShadowsEnabled)
				{
					continue;
				}

				switch(m_CurrLightMode)
				{
				case LightMode::ObservedArea:
				{
					auto const observedArea{ Vector3::Dot(dirToLight, closestHit.normal) };
					if (observedArea >= 0.f)
					{
						finalColor += { observedArea, observedArea, observedArea };
					}
					break;
				}
				case LightMode::Radiance:
				{
					break;
				}
				case LightMode::BRDF:
				{
					break;
				}
				case LightMode::Combined:
				{
					break;
				}
				default:
					break;
				}
			}
		}

		finalColor.MaxToOne();

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
