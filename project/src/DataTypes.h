#ifndef DATATYPES_H
#define DATATYPES_H

#include <vector>
#include "Maths.h"

#include "BVH.h"

namespace dae
{
#pragma region GEOMETRY
	struct Sphere
	{
		Vector3 origin{};
		float radius{};

		unsigned char materialIndex{ 0 };
	};

	struct Plane
	{
		Vector3 origin{};
		Vector3 normal{};

		unsigned char materialIndex{ 0 };
	};

	enum class TriangleCullMode : uint8_t
	{
		FrontFaceCulling,
		BackFaceCulling,
		NoCulling
	};

	struct Triangle
	{
		Vector3 v0{};
		Vector3 v1{};
		Vector3 v2{};

		Vector3 normal{};

		TriangleCullMode cullMode{};
		uint8_t materialIndex{};

		Triangle() = default;
		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2, const Vector3& _normal) :
			v0{ _v0 }, v1{ _v1 }, v2{ _v2 }, normal{ _normal.Normalized() } {}

		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2) :
			v0{ _v0 }, v1{ _v1 }, v2{ _v2 }
		{
			const Vector3 edgeV0V1 = v1 - v0;
			const Vector3 edgeV0V2 = v2 - v0;
			normal = Vector3::Cross(edgeV0V1, edgeV0V2).Normalized();
		}
	};

	struct TriangleMesh
	{
		std::vector<Vector3> positions{}; //vertices
		std::vector<Vector3> normals{}; //normals (1 normal per 3 indices)
		std::vector<int> indices{}; //indices; this contains the index in the positions array to avoid duplicate positions for shared vertices

		Matrix rotationTransform{};
		Matrix translationTransform{};
		Matrix scaleTransform{};

		Vector3 minAABB{};
		Vector3 maxAABB{};

		std::vector<Vector3> transformedPositions{};
		std::vector<Vector3> transformedNormals{};

		Vector3 transformedMinAABB{};
		Vector3 transformedMaxAABB{};

		std::vector<BVHNode> bvh{}; //the mesh BVH, bvh[0] == root
 
		uint8_t materialIndex{};

		TriangleCullMode cullMode{ TriangleCullMode::BackFaceCulling };
		bool isDirty{ false }; //are the transforms currently 'dirty'; whe this is set, the transforms (and bvh will be updated);

		TriangleMesh() = default;
		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, TriangleCullMode _cullMode) :
			positions(_positions), indices(_indices), cullMode(_cullMode)
		{
			CalculateNormals();
			UpdateTransforms();

			UpdateAABB();
			//InitializeBVH();
		}

		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, const std::vector<Vector3>& _normals, TriangleCullMode _cullMode) :
			positions(_positions), indices(_indices), normals(_normals), cullMode(_cullMode)
		{
			UpdateTransforms();
			UpdateAABB();

			//InitializeBVH();
		}

		void Translate(const Vector3& translation)
		{
			translationTransform = Matrix::CreateTranslation(translation);
			isDirty = true;
		}

		void RotateY(float yaw)
		{
			rotationTransform = Matrix::CreateRotationY(yaw);
			isDirty = true;
		}

		void Scale(const Vector3& scale)
		{
			scaleTransform = Matrix::CreateScale(scale);
			isDirty = true;
		}

		void AppendTriangle(const Triangle& triangle, bool ignoreTransformUpdate = false)
		{
			int startIndex{ static_cast<int>(positions.size()) };

			positions.push_back(triangle.v0);
			positions.push_back(triangle.v1);
			positions.push_back(triangle.v2);

			indices.push_back(startIndex);
			indices.push_back(++startIndex);
			indices.push_back(++startIndex);

			normals.push_back(triangle.normal);

			isDirty = true;

			//Not ideal, but making sure all vertices are updated
			if (!ignoreTransformUpdate)
				UpdateTransforms();
		}

		void CalculateNormals()
		{
			normals.clear();
			normals.reserve(indices.size() / 3);

			for(uint32_t i{0}; i < indices.size(); i+=3) //Every 3 indices == a normal
			{
				auto const a{ positions[indices[i+1]] - positions[indices[i]] };
				auto const b{ positions[indices[i+2]] - positions[indices[i]] };

				auto const normal{ Vector3::Cross(a, b).Normalized() };

				normals.emplace_back(normal);
			}
		}

		void UpdateTransforms(bool forceUpdate = false)
		{
			if (!forceUpdate && !isDirty)
			{
				return;
			}

			//Calculate Final Transform 
			const auto finalTransform{ translationTransform * rotationTransform * scaleTransform };

			transformedPositions.clear();
			transformedPositions.reserve(positions.size());
			for(auto const& pos : positions)
			{
				transformedPositions.emplace_back(finalTransform.TransformPoint(pos));
			}

			transformedNormals.clear();
			transformedNormals.reserve(normals.size());
			for (auto const& n : normals)
			{
				transformedNormals.emplace_back(finalTransform.TransformVector(n));
			}

			UpdateTransformedAABB(finalTransform);

			isDirty = false;
		}

		void UpdateAABB()
		{
			if(positions.size() > 0)
			{
				minAABB = positions[0];
				maxAABB = positions[0];

				for(auto const& p : positions)
				{
					minAABB = Vector3::Min(p, minAABB);
					maxAABB = Vector3::Max(p, maxAABB);
				}
			}
		}

		void UpdateTransformedAABB(const Matrix& finalTransform) noexcept
		{
			Vector3 tMinAABB{ finalTransform.TransformPoint(minAABB) };
			Vector3 tMaxAABB{ tMinAABB };

			Vector3 tAABB{ finalTransform.TransformPoint(maxAABB.x, minAABB.y, minAABB.z) };
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMaxAABB);

			tAABB = finalTransform.TransformPoint(maxAABB.x, minAABB.y, maxAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMaxAABB);

			tAABB = finalTransform.TransformPoint(minAABB.x, minAABB.y, maxAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMaxAABB);

			tAABB = finalTransform.TransformPoint(minAABB.x, maxAABB.y, minAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMaxAABB);

			tAABB = finalTransform.TransformPoint(maxAABB.x, maxAABB.y, minAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMaxAABB);

			tAABB = finalTransform.TransformPoint(maxAABB);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMaxAABB);

			tAABB = finalTransform.TransformPoint(minAABB.x, maxAABB.y, maxAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMaxAABB);

			transformedMinAABB = tMinAABB;
			transformedMaxAABB = tMaxAABB;
		}

		void InitializeBVH()
		{
			bvh.emplace_back(BVHNode{  });
			bvh.reserve(1000); //temporarily just reserve 16 child nodes (and 1 root node)

			bvh[0].BuildBVH(bvh, indices, positions, normals, transformedNormals);
		}
	};
#pragma endregion

#pragma region MISC
	struct Ray
	{
		Vector3 origin{};
		Vector3 direction{};

		float min{ 0.0001f };
		float max{ FLT_MAX };
	};

	struct HitRecord
	{
		Vector3 origin{};
		Vector3 normal{};
		float t = FLT_MAX;

		bool didHit{ false };
		uint8_t materialIndex{ 0 };
	};
#pragma endregion
}

#endif