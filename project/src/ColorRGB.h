#pragma once
#include "MathHelpers.h"

namespace dae
{
	struct ColorRGB
	{
		float r{};
		float g{};
		float b{};

		void MaxToOne()
		{
			const float maxValue = std::max(r, std::max(g, b));
			if (maxValue > 1.f)
				*this /= maxValue;
		}

#pragma region ToneMapping
		//https://graphics-programming.org/resources/tonemapping/index.html
		void ReinhardToneMap()
		{
			
		}

		void ExtendedReinhardToneMap()
		{
			
		}

		void ReinhardLuminaceToneMap()
		{
			
		}

		void ReinhardJolieToneMap()
		{
			
		}

		void ACESToneMap()
		{
			
		}

		void ACESAproxToneMap()
		{
			float static constexpr a{ 2.51f };
			float static constexpr b{ 0.03f };
			float static constexpr c{ 2.43f };
			float static constexpr d{ 0.59f };
			float static constexpr e{ 0.14f };

			(*this) *= 0.6f;
			*this = ((*this) * (a * (*this) + b)) / ((*this) * (c * (*this) + d) + e);
		}
#pragma endregion

		static ColorRGB Lerp(const ColorRGB& c1, const ColorRGB& c2, float factor)
		{
			return { Lerpf(c1.r, c2.r, factor), Lerpf(c1.g, c2.g, factor), Lerpf(c1.b, c2.b, factor) };
		}

		#pragma region ColorRGB (Member) Operators
		const ColorRGB& operator+=(const ColorRGB& c)
		{
			r += c.r;
			g += c.g;
			b += c.b;

			return *this;
		}

		const ColorRGB& operator+(const ColorRGB& c)
		{
			return *this += c;
		}

		ColorRGB operator+(const ColorRGB& c) const
		{
			return { r + c.r, g + c.g, b + c.b };
		}

		const ColorRGB& operator-=(const ColorRGB& c)
		{
			r -= c.r;
			g -= c.g;
			b -= c.b;

			return *this;
		}

		const ColorRGB& operator-(const ColorRGB& c)
		{
			return *this -= c;
		}

		ColorRGB operator-(const ColorRGB& c) const
		{
			return { r - c.r, g - c.g, b - c.b };
		}

		const ColorRGB& operator*=(const ColorRGB& c)
		{
			r *= c.r;
			g *= c.g;
			b *= c.b;

			return *this;
		}

		const ColorRGB& operator*(const ColorRGB& c)
		{
			return *this *= c;
		}

		ColorRGB operator*(const ColorRGB& c) const
		{
			return { r * c.r, g * c.g, b * c.b };
		}

		const ColorRGB& operator/=(const ColorRGB& c)
		{
			r /= c.r;
			g /= c.g;
			b /= c.b;

			return *this;
		}

		const ColorRGB& operator/(const ColorRGB& c)
		{
			return *this /= c;
		}

		ColorRGB operator/(const ColorRGB& c) const
		{
			return {r/c.r, g/c.g, b/c.b};
		}

		const ColorRGB& operator+=(float s)
		{
			r += s;
			g += s;
			b += s;

			return *this;
		}

		const ColorRGB& operator*=(float s)
		{
			r *= s;
			g *= s;
			b *= s;

			return *this;
		}

		const ColorRGB& operator*(float s)
		{
			return *this *= s;
		}

		ColorRGB operator*(float s) const
		{
			return { r * s, g * s,b * s };
		}

		const ColorRGB& operator+(float s)
		{
			return *this += s;
		}

		ColorRGB operator+(float s) const
		{
			return { r + s, g + s, b + s };
		}

		const ColorRGB& operator/=(float s)
		{
			r /= s;
			g /= s;
			b /= s;

			return *this;
		}

		const ColorRGB& operator/(float s)
		{
			return *this /= s;
		}

		const ColorRGB& operator/(float s) const
		{
			ColorRGB c{ *this };
			return  c /= s;
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
		static ColorRGB Red{ 1,0,0 };
		static ColorRGB Blue{ 0,0,1 };
		static ColorRGB Green{ 0,1,0 };
		static ColorRGB Yellow{ 1,1,0 };
		static ColorRGB Cyan{ 0,1,1 };
		static ColorRGB Magenta{ 1,0,1 };
		static ColorRGB White{ 1,1,1 };
		static ColorRGB Black{ 0,0,0 };
		static ColorRGB Gray{ 0.5f,0.5f,0.5f };
	}
}