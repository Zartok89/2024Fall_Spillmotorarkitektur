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