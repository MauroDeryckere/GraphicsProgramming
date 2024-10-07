#pragma once
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Maths.h"
#include "Timer.h"

namespace dae
{
	struct Camera final
	{
	public:
		Vector3 origin{};
		float fovAngle{ 90.f };

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{ 0.f };
		float totalYaw{ 0.f };

		Matrix cameraToWorld{  };

		float movementSpeed{ 3.f };
		float rotationSpeed{ 10.f };

		Camera() = default;

		Camera(Vector3 const&  _origin, float _fovAngle) :
			origin{ _origin },
			fovAngle{ _fovAngle }
		{ }

		Matrix CalculateCameraToWorld()
		{
			Vector3 const r{ Vector3::Cross(Vector3::UnitY, forward) };
			right = r.Normalized();
			up = Vector3::Cross(forward, r).Normalized();

			cameraToWorld = { right, up, forward, origin };

			return cameraToWorld;
		}

		void Update(Timer* pTimer)
		{
			float const deltaTime{ pTimer->GetElapsed() };

			Vector3 movementDir{ };

			//Keyboard Input
			uint8_t const* pKeyboardState{ SDL_GetKeyboardState(nullptr) };
			if(pKeyboardState[SDL_SCANCODE_W])
			{
				movementDir += forward;
			}
			if (pKeyboardState[SDL_SCANCODE_S])
			{
				movementDir -= forward;
			}

			if (pKeyboardState[SDL_SCANCODE_A])
			{
				movementDir -= right;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				movementDir += right;
			}

			if (movementDir != Vector3::Zero)
			{
				movementDir.Normalize();
				origin += (movementDir * movementSpeed * deltaTime);
			}

			//Mouse Input
			int mouseX{};
			int mouseY{};
			uint32_t const mouseState{ SDL_GetRelativeMouseState(&mouseX, &mouseY) };

			if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT))
			{
				UpdateCameraDirection(static_cast<float>(mouseX), static_cast<float>(mouseY), deltaTime);
			}
		}

	private:
		void UpdateCameraDirection(float deltaX, float deltaY, float deltaTime)
		{
			if (deltaX == 0.f && deltaY == 0.f)
			{
				return;
			}

			totalYaw -= deltaX * rotationSpeed * deltaTime;
			totalPitch -= deltaY * rotationSpeed * deltaTime;  //Invert pitch for natural mouse movement

			auto const m{ Matrix::CreateRotation(TO_RADIANS * totalPitch, TO_RADIANS * totalYaw, 0.f) };

			forward = m.TransformVector(Vector3::UnitZ);
			forward.Normalize();
		}
	};
}
