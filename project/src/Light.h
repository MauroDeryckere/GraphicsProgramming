#ifndef LIGHT_H
#define LIGHT_H

#include "Maths.h"
#include "DataTypes.h"

#pragma region LIGHT
namespace dae
{
	enum class LightShape : uint8_t
	{
		None,
		Triangular
		//Rectangular TODO
		//...
	};

	enum class LightType : uint8_t
	{
		Point,
		Area, //Just a simple round light for now
		Directional
	};

	struct Light
	{
		Vector3 origin{};
		float intensity{};

		ColorRGB color{};

		LightType type{};

		Vector3 direction{}; //this is the normal for the triangular light, for a directional light it is the direction

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

			return {dir , dis };
		}
		case LightType::Directional:
			return { -light.direction, std::numeric_limits<float>::max() };
		}

		return {};
	}

	//Light point is the (sampled) point on the light or origin point of the light
	inline ColorRGB GetRadiance(const Light& light, const Vector3& lightPoint, const HitRecord& hitRecord) noexcept
	{
		switch (light.type)
		{
			case LightType::Point:
			{
				return light.color * light.intensity / (Vector3::Dot(light.origin - hitRecord.origin, light.origin - hitRecord.origin));
			}
			case LightType::Area:
			{
				auto dir{ lightPoint - hitRecord.origin };
				auto ddotn = Vector3::Dot(-light.direction, hitRecord.normal);

				if (ddotn < 0.f)
				{
					return {};
				}

				float dSq = (lightPoint - hitRecord.origin).SqrMagnitude();
				float G = ddotn / (dSq);

				return light.color * light.intensity * G;
			}
			case LightType::Directional:
			{
				return light.color * light.intensity;
			}
		}

		return {};
	}

	//Normal is the surface normal
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
				return Vector3::Dot(-light.direction, normal);
			}
		}

		return {};
	}
} 
#pragma endregion

#endif