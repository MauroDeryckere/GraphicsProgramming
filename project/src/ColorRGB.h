#pragma once
#include "MathHelpers.h"

namespace dae
{
	struct ColorRGB final
	{
		float r{};
		float g{};
		float b{};

		constexpr ColorRGB() = default;
		constexpr ColorRGB(float _r, float _g, float _b):
		r{ _r }, g{ _g }, b{ _b } {}

		constexpr float Luminance() const
		{
			return 0.2126f * r + 0.7152f * g + 0.0722f * b;
		}

		constexpr void ChangeLuminance(float luminance)
		{
			float const l{ this->Luminance() };

			(*this) = (*this) *  (luminance / l);
		}

		constexpr void MaxToOne()
		{
			const float maxValue = std::max(r, std::max(g, b));
			if (maxValue > 1.f)
				*this /= maxValue;
		}

		static const ColorRGB& Lerp(const ColorRGB& c1, const ColorRGB& c2, float factor)
		{
			return { Lerpf(c1.r, c2.r, factor), Lerpf(c1.g, c2.g, factor), Lerpf(c1.b, c2.b, factor) };
		}

		#pragma region ColorRGB (Member) Operators
		constexpr const ColorRGB& operator+=(const ColorRGB& c)
		{
			r += c.r;
			g += c.g;
			b += c.b;

			return *this;
		}

		constexpr ColorRGB operator+(const ColorRGB& c) const
		{
			return { r + c.r, g + c.g, b + c.b };
		}

		constexpr const ColorRGB& operator-=(const ColorRGB& c)
		{
			r -= c.r;
			g -= c.g;
			b -= c.b;

			return *this;
		}

		constexpr ColorRGB operator-(const ColorRGB& c) const
		{
			return { r - c.r, g - c.g, b - c.b };
		}

		constexpr const ColorRGB& operator*=(const ColorRGB& c)
		{
			r *= c.r;
			g *= c.g;
			b *= c.b;

			return *this;
		}

		constexpr ColorRGB operator*(const ColorRGB& c) const
		{
			return { r * c.r, g * c.g, b * c.b };
		}

		constexpr const ColorRGB& operator/=(const ColorRGB& c)
		{
			r /= c.r;
			g /= c.g;
			b /= c.b;

			return *this;
		}
		constexpr ColorRGB operator/(const ColorRGB& c) const
		{
			return {r/c.r, g/c.g, b/c.b};
		}


		constexpr const ColorRGB& operator*=(float s)
		{
			r *= s;
			g *= s;
			b *= s;

			return *this;
		}

		constexpr ColorRGB operator*(float s) const
		{
			return { r * s, g * s,b * s };
		}

		constexpr const ColorRGB& operator+=(float s)
		{
			r += s;
			g += s;
			b += s;

			return *this;
		}

		constexpr const ColorRGB& operator+(float s)
		{
			return *this += s;
		}

		constexpr ColorRGB operator+(float s) const
		{
			return { r + s, g + s, b + s };
		}

		constexpr const ColorRGB& operator/=(float s)
		{
			r /= s;
			g /= s;
			b /= s;

			return *this;
		}

		constexpr ColorRGB operator/(float s) const
		{
			return { r / s, g / s, b / s };
		}

		friend ColorRGB operator*(float s, const ColorRGB& color);
		friend ColorRGB operator-(float s, const ColorRGB& color);
		#pragma endregion
	};

	//ColorRGB (Global) Operators
	inline ColorRGB operator*(float s, const ColorRGB& c)
	{
		return c * s;
	}
	inline ColorRGB operator-(float s, const ColorRGB& c)
	{
		return { s-c.r, s-c.g, s-c.b };
	}

	namespace colors
	{
		static constexpr ColorRGB Red{ 1,0,0 };
		static constexpr ColorRGB Blue{ 0,0,1 };
		static constexpr ColorRGB Green{ 0,1,0 };
		static constexpr ColorRGB Yellow{ 1,1,0 };
		static constexpr ColorRGB Cyan{ 0,1,1 };
		static constexpr ColorRGB Magenta{ 1,0,1 };
		static constexpr ColorRGB White{ 1,1,1 };
		static constexpr ColorRGB Black{ 0,0,0 };
		static constexpr ColorRGB Gray{ 0.5f,0.5f,0.5f };
	}

#pragma region ToneMapping
	static void ReinhardJolieToneMap(ColorRGB& color)
	{
		float const l{ color.Luminance() };
		auto const tv{ color / (colors::White + color) };
		auto const c1{ color / (1.0f + l) };

		color = ColorRGB{ Lerpf(c1.r, tv.r, tv.r), Lerpf(c1.g, tv.g, tv.g), Lerpf(c1.b, tv.b, tv.b) };
	}

	//https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
	static void ACESAproxToneMap(ColorRGB& color)
	{
		float static constexpr a{ 2.51f };
		float static constexpr b{ 0.03f };
		float static constexpr c{ 2.43f };
		float static constexpr d{ 0.59f };
		float static constexpr e{ 0.14f };

		color *= .6f;

		color = (color * (a * color + b)) / (color * (c * color + d) + e);

		color.r = std::clamp(color.r, 0.f, 1.f);
		color.g = std::clamp(color.g, 0.f, 1.f);
		color.b = std::clamp(color.b, 0.f, 1.f);
	}

	//not aprox
	//https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl
#pragma endregion

}