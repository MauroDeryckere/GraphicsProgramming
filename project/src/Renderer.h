#pragma once

#include <cstdint>
#include <vector>
#include <iostream>

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		bool SaveBufferToImage() const;

		void CycleLighMode() noexcept
		{
			auto curr{ static_cast<uint8_t>(m_CurrLightMode) };
			++curr %= static_cast<uint8_t>(LightMode::COUNT);

			m_CurrLightMode = static_cast<LightMode>(curr);
		}

		void ToggleShadows() noexcept { m_ShadowsEnabled = !m_ShadowsEnabled; }

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};

		//Has to be updated when window is resized (not possible at the moment)
		std::vector<int> m_Pixels{};

		enum class LightMode : uint8_t
		{
			ObservedArea, //Lambert cosine law
			Radiance, //Indicent Radiance
			BRDF, //Scattering of the light
			Combined, //ObservedArea * Radiance * BRDF
			COUNT
		};

		LightMode m_CurrLightMode{ LightMode::Combined };
		bool m_ShadowsEnabled{ true };
	};
}
