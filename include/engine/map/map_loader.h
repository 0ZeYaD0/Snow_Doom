#pragma once

#include <engine/physics/aabb.h>
#include <engine/graphics/mesh.h>
#include <engine/core/defines.h>

#include <string>

using std::string, std::vector;

struct MapBlock
{
    glm::vec3 pos;
    glm::vec3 size;
};

struct LoadMap
{
    vector<MapBlock> blocks;
    vector<AABB> colliders;
    vector<Mesh> meshes;
};

namespace MapLoader
{
    LoadMap Load(const string &filepath);
}