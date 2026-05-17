#pragma once

#include <engine/physics/aabb.h>
#include <engine/graphics/mesh.h>
#include <engine/core/defines.h>
#include <engine/graphics/texture.h>
#include <game/entities/pickup.h>

#include <string>
#include <memory>
using std::string, std::vector;

struct MapBlock
{
    glm::vec3 pos;
    glm::vec3 size;
};

struct EnemySpawnData
{
    glm::vec3 pos;
    std::shared_ptr<Texture> texture;
};

struct DoorSpawnData
{
    glm::vec3 pos;
    glm::vec3 size;
    std::string req_key;
    std::shared_ptr<Texture> texture;
    bool should_tile;
};

struct PickupSpawnData
{
    glm::vec3 pos;
    PickupType type;
    std::string item_id;
    std::shared_ptr<Texture> texture;
};

struct MapEntity
{
    Mesh mesh;
    std::shared_ptr<Texture> texture;
    MapEntity() : mesh(), texture(nullptr) {}
};

struct LoadMap
{
    vector<MapBlock> blocks;
    vector<AABB> colliders;
    vector<MapEntity> entities;
    vector<EnemySpawnData> enemies;
    vector<DoorSpawnData> doors;
    vector<PickupSpawnData> pickups;
};

namespace MapLoader
{
    Mesh BuildCubeMesh(glm::vec3 pos, glm::vec3 size, bool tile);
    LoadMap Load(const string &filepath);
}