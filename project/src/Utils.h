#pragma once
#include <iostream>
#include <fstream>
#include "Maths.h"
#include "DataTypes.h"

#include <random>
#include <limits>

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{			
			auto const a{ Vector3::Dot(ray.direction, ray.direction) };
			auto const b{ Vector3::Dot(2 * ray.direction , (ray.origin - sphere.origin))};
			auto const c{ Vector3::Dot(ray.origin - sphere.origin, ray.origin - sphere.origin) - sphere.radius * sphere.radius };

			auto const d{ b * b - 4 * a * c };

			if (d <= 0)
			{
				hitRecord.didHit = false;
				return false;
			}

			float t{ (-b - sqrt(d)) / 2 * a };

			if (t > ray.max || t < ray.min)
			{
				t = (-b + sqrt(d)) / 2 * a;
			}

			if (t > ray.max || t < ray.min)
			{
				hitRecord.didHit = false;
				return false;
			}

			if (ignoreHitRecord)
			{
				return true;
			}

			hitRecord.didHit = true;
			hitRecord.materialIndex = sphere.materialIndex;
			hitRecord.origin = ray.origin + t * ray.direction;

			hitRecord.t = t;

			hitRecord.normal = (hitRecord.origin - sphere.origin).Normalized();

			return true;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{	
			float const t{ Vector3::Dot((plane.origin - ray.origin), plane.normal) / Vector3::Dot(ray.direction, plane.normal) };
			
			if (t < ray.min || t > ray.max)
			{
				return false;
			}

			if (ignoreHitRecord)
			{
				return true;
			}

			Vector3 const p{ ray.origin + ray.direction * t};

			hitRecord.didHit = true;
			hitRecord.materialIndex = plane.materialIndex;
			hitRecord.origin = p;
			
			hitRecord.t = t;
			hitRecord.normal = plane.normal;

			return true;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			auto const origin{ (triangle.v0 + triangle.v1 + triangle.v2) / 3.f };
			auto const dotProd{ Vector3::Dot(triangle.normal, ray.direction) };

			if (AreEqual(dotProd, 0.f))
			{
				return false;
			}

			if (ignoreHitRecord)
			{
				switch (triangle.cullMode)
				{
				case TriangleCullMode::BackFaceCulling:
					if (dotProd < 0.f)
					{
						return false;
					}
					break;
				case TriangleCullMode::FrontFaceCulling:
					if (dotProd > 0.f)
					{
						return false;
					}
					break;
				case TriangleCullMode::NoCulling:
					break;
				default:
					break;
				}
			}
			else
			{
				switch (triangle.cullMode)
				{
				case TriangleCullMode::BackFaceCulling:
					if (dotProd > 0.f)
					{
						return false;
					}
					break;
				case TriangleCullMode::FrontFaceCulling:
					if (dotProd < 0.f)
					{
						return false;
					}
					break;
				case TriangleCullMode::NoCulling:
					break;
				default:
					break;
				}
			}

			float const t{ Vector3::Dot((origin - ray.origin), triangle.normal) / Vector3::Dot(ray.direction, triangle.normal) };
			if (t < ray.min ||  t > ray.max)
			{
				return false;
			}

			Vector3 const point{ ray.origin + ray.direction * t };

			auto e{triangle.v0 - triangle.v2};
			auto p{ point - triangle.v2 };

			if (Vector3::Dot(Vector3::Cross(e, p), triangle.normal) < 0.f)
			{
				return false;
			}

			e = triangle.v1 - triangle.v0;
			p = point - triangle.v0;

			if (Vector3::Dot(Vector3::Cross(e, p), triangle.normal) < 0.f)
			{
				return false;
			}

			e = triangle.v2 - triangle.v1;
			p = point - triangle.v1;

			if (Vector3::Dot(Vector3::Cross(e, p), triangle.normal) < 0.f)
			{
				return false;
			}

			hitRecord.origin = point;
			hitRecord.normal = triangle.normal;
			hitRecord.t = t;
			hitRecord.didHit = true;
			hitRecord.materialIndex = triangle.materialIndex;

			return true;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			HitRecord closestHitRecord{ };

			for (uint32_t i{ 0 }; i < mesh.indices.size(); i += 3) //Every 3 indices == a triangle
			{
				Triangle t{ mesh.transformedPositions[mesh.indices[i]],
							mesh.transformedPositions[mesh.indices[i+1]],
							mesh.transformedPositions[mesh.indices[i+2]],
							mesh.transformedNormals[i/3]
						 };


				t.cullMode = mesh.cullMode;
				t.materialIndex = mesh.materialIndex;

				HitRecord temp{  };
				if (HitTest_Triangle(t, ray, temp, ignoreHitRecord))
				{
					if (temp.t < closestHitRecord.t)
					{
						closestHitRecord = temp;
					}
				}
			}

			hitRecord = closestHitRecord;

			return closestHitRecord.didHit;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof())
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)

		template<typename T>
		constexpr T Random(T min, T max) noexcept
		requires (std::is_floating_point_v<T> || std::is_integral_v<T>)
		{
			if constexpr (std::is_floating_point_v<T>)
			{
				thread_local std::uniform_real_distribution<T> distribution(min, max);
				thread_local std::mt19937 generator;

				return distribution(generator);
			}

			else if constexpr (std::is_integral_v<T>)
			{
				thread_local std::uniform_int_distribution<T> distribution(min, max);
				thread_local std::mt19937 generator;
				return distribution(generator);
			}

			return std::numeric_limits<T>::min();
		}
	}
}