#pragma once

#include <cstdint>
#include <vector>
#include <iostream>

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class ColorRGB;
	class Vector3;
	class Scene;
	struct Light;
	struct HitRecord;

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

		void CycleSampleMode() noexcept
		{
			auto curr{ static_cast<uint8_t>(m_CurrSampleMode) };
			++curr %= static_cast<uint8_t>(SampleMode::COUNT);

			m_CurrSampleMode = static_cast<SampleMode>(curr);
		}

		void IncreaseSamples() noexcept { m_SampleCount *= 2; }
		void DecreaseSamples() noexcept { m_SampleCount = std::max<uint32_t>(m_SampleCount / 2, 1); }

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};

		//Has to be updated when window is resized (not possible at the moment)
		std::vector<uint32_t> m_Pixels{ };

		enum class LightMode : uint8_t
		{
			ObservedArea, //Lambert cosine law
			Radiance, //Indicent Radiance
			BRDF, //Scattering of the light
			Combined, //ObservedArea * Radiance * BRDF
			COUNT
		}; 
		LightMode m_CurrLightMode{ LightMode::Combined }; //Cycle through with F3
		bool m_ShadowsEnabled{ true }; //Switched on/off with F2

		enum class SampleMode : uint8_t
		{
			RandomSquare,
			UniformSquare,
			COUNT
		};
		SampleMode m_CurrSampleMode{ SampleMode::UniformSquare }; //Cycle through with F4
		uint32_t m_SampleCount{ 1 }; //Samples per pixel; Decrease with F5, Increase with F6
		uint32_t m_LightSamples{ 10 }; //Samples per area light

		[[nodiscard]] ColorRGB CalculateIllumination(Scene* pScene, const Light& light, const HitRecord& closestHit, const Vector3& viewDir) const noexcept;

		Vector3 SampleRay(uint32_t currSample) const noexcept;

		Vector3 SampleRandomSquare() const noexcept;
		Vector3 SampleUniformSquare(uint32_t currSample) const noexcept;

		void BoxFilter(ColorRGB& c) const noexcept;
		//GaussFilter(); //TODO
	};
}
