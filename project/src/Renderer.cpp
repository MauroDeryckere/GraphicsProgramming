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

#include <iostream>

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera{ pScene->GetCamera() };
	auto const& materials{ pScene->GetMaterials() };
	auto const& lights { pScene->GetLights() };

	float const aspectRatio{ m_Width / static_cast<float>(m_Height) };
	float const fov{ tan(camera.fovAngle * TO_RADIANS/2) }; //TODO -calculate only when updating

	Matrix const cameraToWorld{ camera.CalculateCameraToWorld() };

	for (int px{0}; px < m_Width; ++px)
	{
		float const x{ ( 2 * (px + .5f) / m_Width - 1) * aspectRatio * fov };
		
		for (int py{0}; py < m_Height; ++py)
		{
			float const y{ (1 - 2 * (py + .5f) / m_Height) * fov };

			Vector3 const dirViewSpace{ x , y, 1.f};
			Vector3 const dirWorldSpace{ (cameraToWorld.TransformVector(dirViewSpace)).Normalized() };

			Ray const viewRay{ cameraToWorld.GetTranslation() , dirWorldSpace };
			
			HitRecord closestHit{ };
			pScene->GetClosestHit(viewRay, closestHit);

			ColorRGB finalColor{ };

			if (closestHit.didHit)
			{
				finalColor = materials[closestHit.materialIndex]->Shade();

				for (auto const& light : lights)
				{
					auto dirToLight{ LightUtils::GetDirectionToLight(light, closestHit.origin) };
					auto const distance{ dirToLight.Normalize() };

					Ray const shadowRay{ closestHit.origin, dirToLight, 0.0001f, distance };

					if (pScene->DoesHit(shadowRay))
					{
						finalColor *= .5f;
					}
				}
			}

			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
