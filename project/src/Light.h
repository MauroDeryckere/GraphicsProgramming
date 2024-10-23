#ifndef LIGHT_H
#define LIGHT_H

#include "Maths.h"

#pragma region LIGHT
namespace dae
{
	enum class LightType : uint8_t
	{
		Point,
		Directional
	};

	struct Light final
	{
		Vector3 origin{};
		Vector3 direction{};
		ColorRGB color{};
		float intensity{};

		LightType type{};
	};

	//Direction from target to light
	inline Vector3 GetDirectionToLight(const Light& light, const Vector3& origin)
	{
		switch (light.type)
		{
		case LightType::Point:
			return light.origin - origin;
		case LightType::Directional:
			return {}; //todo
		}

		return {};
	}

	inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
	{
		switch (light.type)
		{
			case LightType::Point:
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
			case LightType::Directional:
			{
				return {};
			}
		}

		return {};
	}
	
} 
#pragma endregion

#endif