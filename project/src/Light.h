#ifndef LIGHT_H
#define LIGHT_H

#include "Maths.h"

#pragma region LIGHT
namespace dae
{
	enum class LightType : uint8_t
	{
		Point,
		Area, //Just a simple round light for now
		Directional
	};

	enum class LightShape : uint8_t
	{
		None,
		Triangular
	};

	struct Light
	{
		Vector3 origin{};
		float intensity{};

		ColorRGB color{};

		LightType type{};

		Vector3 direction{};

		float radius{};
		std::vector<Vector3> vertices{};

		LightShape shape{};

		//infintely small or infintely far away lights do not require the calculations for soft shadows
		[[nodiscard]] bool HasSoftShadows() const noexcept
		{
			return (type != LightType::Directional && type != LightType::Point);
		}
	};

	//returns direction from target to light and the distance to the light
	//Light point is the (sampled) point on the light or origin point of the light
	inline std::pair<Vector3, float> GetDirectionToLight(const Light& light, const Vector3& lightPoint, const Vector3& hitOrigin) noexcept
	{
		switch (light.type)
		{
		case LightType::Point:
		{
			auto dir { lightPoint - hitOrigin };
			float const dis{ dir.Normalize() };

			return { dir, dis };
		}
		case LightType::Area:
		{
			auto dir{ lightPoint - hitOrigin };
			float const dis{ dir.Normalize() };

			return { dir, dis };
		}
		case LightType::Directional:
			return {}; //todo
		}

		return {};
	}

	//Light point is the (sampled) point on the light or origin point of the light
	inline ColorRGB GetRadiance(const Light& light, const Vector3& lightPoint, const Vector3& target) noexcept
	{
		switch (light.type)
		{
			case LightType::Point:
			{
				return light.color * light.intensity / (Vector3::Dot(light.origin - target, light.origin - target));
			}
			case LightType::Area:
			{
				return light.color * light.intensity / (Vector3::Dot(light.origin - target, light.origin - target));
			}
			case LightType::Directional:
			{
				return light.color * light.intensity;
			}
		}

		return {};
	}

	inline float GetObservedArea(const Light& light, const Vector3& dirToLight, const Vector3& normal) noexcept
	{
		switch (light.type)
		{
			case LightType::Point:
			{
				return Vector3::Dot(dirToLight, normal);
			}
			case LightType::Area:
			{
				return Vector3::Dot(dirToLight, normal);
			}
			case LightType::Directional:
			{
				return {}; //todo
			}
		}

		return {};
	}
} 
#pragma endregion

#endif