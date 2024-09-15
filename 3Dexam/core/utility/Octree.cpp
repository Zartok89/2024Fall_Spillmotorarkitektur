#include "Octree.h"

OctreeNode::OctreeNode(const AABB& boundary, int capacity)
{
	mBoundary = boundary;
	mCapacity = capacity;
	mDivided = false;
}

bool OctreeNode::Insert(const std::shared_ptr<Actor>& actor)
{
	if (!mBoundary.Contains(actor->GetActorPosition()))
	{
		return false;
	}

	if (mActors.size() < mCapacity && !mDivided)
	{
		mActors.push_back(actor);
		return true;
	}

	if (!mDivided)
	{
		Subdivide();
	}

	for (int i = 0; i < 8; ++i)
	{
		if (mChildren[i]->Insert(actor))
		{
			return true;
		}
	}

	return false;
}

void OctreeNode::Subdivide()
{
	glm::vec3 newHalf = mBoundary.halfDimension * 0.5f;
	for (int i = 0; i < 8; ++i)
	{
		glm::vec3 offset(
			(i & 1 ? newHalf.x : -newHalf.x),
			(i & 2 ? newHalf.y : -newHalf.y),
			(i & 4 ? newHalf.z : -newHalf.z));
		AABB childBoundary{ mBoundary.center + offset, newHalf };
		mChildren[i] = std::make_unique<OctreeNode>(childBoundary, mCapacity);
	}
	mDivided = true;

	// Re-insert existing actors into children
	for (const auto& actor : mActors)
	{
		for (int i = 0; i < 8; ++i)
		{
			if (mChildren[i]->Insert(actor))
			{
				break;
			}
		}
	}
	mActors.clear();
}

void OctreeNode::Query(const AABB& range, std::vector<std::shared_ptr<Actor>>& found) const
{
	if (!mBoundary.Intersects(range))
	{
		return;
	}

	for (const auto& actor : mActors)
	{
		if (range.Contains(actor->GetActorPosition()))
		{
			found.push_back(actor);
		}
	}

	if (mDivided)
	{
		for (int i = 0; i < 8; ++i)
		{
			mChildren[i]->Query(range, found);
		}
	}
}

void OctreeNode::Clear()
{
	mActors.clear();
	if (mDivided)
	{
		for (int i = 0; i < 8; ++i)
		{
			mChildren[i]->Clear();
			mChildren[i].reset();
		}
		mDivided = false;
	}
}
//
//void OctreeNode::CollectAABBs(const std::unique_ptr<OctreeNode>& node, std::vector<AABB>& aabbs)
//{
//	if (!node) return;
//
//	// Add the current node's boundary
//	aabbs.push_back(node->mBoundary);
//
//	// If divided, traverse children
//	if (node->mDivided) {
//		for (int i = 0; i < 8; ++i) {
//			CollectAABBs(node->mChildren[i], aabbs);
//		}
//	}
//}
//
//std::vector<glm::vec3> OctreeNode::GenerateWireframeCube(const AABB& box)
//{
//	std::vector<glm::vec3> vertices;
//
//	// Calculate the 8 corners of the cube
//	glm::vec3 min = box.center - box.halfDimension;
//	glm::vec3 max = box.center + box.halfDimension;
//
//	glm::vec3 corners[8] = {
//		glm::vec3(min.x, min.y, min.z),
//		glm::vec3(max.x, min.y, min.z),
//		glm::vec3(max.x, max.y, min.z),
//		glm::vec3(min.x, max.y, min.z),
//		glm::vec3(min.x, min.y, max.z),
//		glm::vec3(max.x, min.y, max.z),
//		glm::vec3(max.x, max.y, max.z),
//		glm::vec3(min.x, max.y, max.z)
//	};
//
//	// Define the 12 edges of the cube
//	int edgeIndices[24] = {
//		0, 1, 1, 2, 2, 3, 3, 0, // Bottom edges
//		4, 5, 5, 6, 6, 7, 7, 4, // Top edges
//		0, 4, 1, 5, 2, 6, 3, 7  // Side edges
//	};
//
//	// Populate the vertices based on edges
//	for (int i = 0; i < 24; i += 2) {
//		vertices.push_back(corners[edgeIndices[i]]);
//		vertices.push_back(corners[edgeIndices[i + 1]]);
//	}
//
//	return vertices;
//}
//
//void OctreeNode::RenderOctree(const std::unique_ptr<OctreeNode>& octree)
//{
//	// Collect all AABBs
//	std::vector<AABB> aabbs;
//	CollectAABBs(octree, aabbs);
//
//	// Generate wireframe vertices
//	std::vector<glm::vec3> wireframeVertices;
//	for (const auto& box : aabbs) {
//		auto cubeVertices = GenerateWireframeCube(box);
//		wireframeVertices.insert(wireframeVertices.end(), cubeVertices.begin(), cubeVertices.end());
//	}
//}