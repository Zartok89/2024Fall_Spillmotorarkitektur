#pragma once

#include "glm/glm.hpp"
#include <vector>
#include <memory>
#include <graphical/Actor.h>

struct AABB {  
    glm::vec3 center;  
    glm::vec3 halfDimension;  

    bool Contains(const glm::vec3& point) const {  
        return (abs(point.x - center.x) <= halfDimension.x &&  
                abs(point.y - center.y) <= halfDimension.y &&  
                abs(point.z - center.z) <= halfDimension.z);  
    }  

    bool Intersects(const AABB& other) const {  
        return (abs(center.x - other.center.x) <= (halfDimension.x + other.halfDimension.x)) &&  
               (abs(center.y - other.center.y) <= (halfDimension.y + other.halfDimension.y)) &&  
               (abs(center.z - other.center.z) <= (halfDimension.z + other.halfDimension.z));  
    }  
};  

class OctreeNode {  
public:  
    AABB mBoundary;  
    int mCapacity;  
    std::vector<std::shared_ptr<Actor>> mActors;  
    bool mDivided;  
    std::unique_ptr<OctreeNode> mChildren[8];  

    OctreeNode(const AABB& boundary, int capacity); 

    bool Insert(const std::shared_ptr<Actor>& actor);  
    void Subdivide();  
    void Query(const AABB& range, std::vector<std::shared_ptr<Actor>>& found) const;
    void Clear();

    //void CollectAABBs(const std::unique_ptr<OctreeNode>& node, std::vector<AABB>& aabbs);
    //std::vector<glm::vec3> GenerateWireframeCube(const AABB& box);
    //void RenderOctree(const std::unique_ptr<OctreeNode>& octree);
};  