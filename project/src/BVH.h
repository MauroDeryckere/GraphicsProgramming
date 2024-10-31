#ifndef BVH_H
#define BVH_H

#include <cassert>
#include <stdint.h>
#include <limits>
#include <vector>
#include "Vector3.h"

namespace dae
{
	struct BVHNode final
	{
		Vector3 aabbMin{};
		Vector3 aabbMax{};

//		uint32_t leftChild{ 0 }; //rightChild is always leftChild + 1 and always exists

		uint32_t leftFirst{ 0 };
		uint32_t triangleCount{ 0 };

		bool IsLeaf() const
		{ 
			return triangleCount > 0; 
		}

		void BuildBVH(std::vector<BVHNode>& bvh, std::vector<int>& indices, std::vector<Vector3>const& vertices, std::vector<Vector3>& normals, std::vector<Vector3>& transformedNormals)
		{
			static constexpr uint32_t rootNodeIdx{ 0 };

			BVHNode& root{ bvh[rootNodeIdx] };
			root.triangleCount = normals.size();

			//Assign all to root node
			UpdateNodeBounds(bvh, indices, vertices, rootNodeIdx);
			//Recursively subdivide
			SubDivide(bvh, indices, vertices, rootNodeIdx, normals, transformedNormals);
		}

		void UpdateNodeBounds(std::vector<BVHNode>& bvh, std::vector<int>const& indices, std::vector<Vector3>const& vertices, uint32_t nodeIdx)
		{
			assert(bvh[nodeIdx].IsLeaf());

			BVHNode& node{ bvh[nodeIdx] };
			node.aabbMin = { 1e30f, 1e30f, 1e30f };
			node.aabbMax = { -1e30f, -1e30f, -1e30f };
			
			uint32_t const first{ node.leftFirst };
			for (uint32_t i{ 0 }; i < node.triangleCount; ++i)
			{
				//every 3 indices == a triangle
				auto const offset{ (first + i) * 3 };

				node.aabbMin = Vector3::Min(node.aabbMin, vertices[indices[offset]]);
				node.aabbMin = Vector3::Min(node.aabbMin, vertices[indices[offset + 1]]);
				node.aabbMin = Vector3::Min(node.aabbMin, vertices[indices[offset + 2]]);
				node.aabbMax = Vector3::Max(node.aabbMax, vertices[indices[offset]]);
				node.aabbMax = Vector3::Max(node.aabbMax, vertices[indices[offset + 1]]);
				node.aabbMax = Vector3::Max(node.aabbMax, vertices[indices[offset + 2]]);
			}
		}

		void SubDivide(std::vector<BVHNode>& bvh, std::vector<int>& indices, std::vector<Vector3>const& vertices, uint32_t nodeIdx, std::vector<Vector3>& normals, std::vector<Vector3>& transformedNormals)
		{
			BVHNode& node{ bvh[nodeIdx] };

			if (node.triangleCount <= 2)
			{
				return;
			}

			//Splitting plane axis
			Vector3 const extent{ node.aabbMax - node.aabbMin };
			int axis{ 0 };
			if (extent.y > extent.x)
			{
				axis = 1;
			}
			if (extent.z > extent[axis])
			{
				axis = 2;
			}

			float const splitPos{ node.aabbMin[axis] + extent[axis] * .5f };

			uint32_t i{ node.leftFirst };
			uint32_t j{ i + node.triangleCount - 1 };

			//in place splitting of groups
			while(i <= j)
			{
				//triangles[0] == indices[0, 1, 2] and so on
				auto const idx{ i * 3 };
				//Center is currently not stored inside the mesh, could be stored if necessary
				Vector3 const center{ (vertices[indices[idx]] + vertices[indices[idx + 1]] + vertices[indices[idx + 2]]) / 3 };

				if (center[axis] < splitPos)
				{
					i++;
				}
				else
				{
					//Swap the indices - Example: indices [0, 1, 2] are swapped with [6, 7, 8] - with the way we currently store data this would mess up the normals
					//So also swap the normal; indices[0, 1, 2] == normal [0] so say we swap indices 0-2 with indices 6-8 we swap normal 0 with normal 2
					//Do the same with the transformxedNormals to avoid recalculating all normals
					//this can be avoided by changing the mesh struct in the future - store a vector with triangles and work with the triangleIndex in the bvh

					auto const idx2{ (j * 3) };
					std::swap(indices[idx], indices[idx2]);
					std::swap(indices[idx + 1], indices[idx2 + 1]);
					std::swap(indices[idx + 2], indices[idx2 + 2]);

					std::swap(normals[i], normals[j]);
					std::swap(transformedNormals[i], transformedNormals[j]);

					--j;
				}
			}

			//Abort split if one of the sides is empty
			uint32_t leftCount{ i - node.leftFirst };
			if (leftCount == 0 || leftCount == node.triangleCount)
			{
				return;
			}

			//Create child nodes
			bvh.emplace_back(BVHNode{});
			auto const leftChildIdx = bvh.size() - 1;
			bvh.emplace_back(BVHNode{});
			auto const rightChildIdx = bvh.size() - 1;

			bvh[leftChildIdx].leftFirst = node.leftFirst;
			bvh[leftChildIdx].triangleCount = leftCount;
			bvh[rightChildIdx].leftFirst = i;
			bvh[rightChildIdx].triangleCount = node.triangleCount - leftCount;

			node.leftFirst = leftChildIdx;
			node.triangleCount = 0;
			UpdateNodeBounds(bvh, indices, vertices, leftChildIdx);
			UpdateNodeBounds(bvh, indices, vertices, rightChildIdx);
			//Recurse
			SubDivide(bvh, indices, vertices, leftChildIdx,normals, transformedNormals);
			SubDivide(bvh, indices, vertices, rightChildIdx, normals, transformedNormals);
		}
	};
}

#endif