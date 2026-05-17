#include <engine/map/map_loader.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

namespace MapLoader
{
    Mesh BuildCubeMesh(glm::vec3 pos, glm::vec3 size, bool tile = true)
    {
        glm::vec3 h = size * 0.5f;

        glm::vec3 corners[8] = {
            pos + glm::vec3(-h.x, -h.y, -h.z),
            pos + glm::vec3(h.x, -h.y, -h.z),
            pos + glm::vec3(h.x, h.y, -h.z),
            pos + glm::vec3(-h.x, h.y, -h.z),
            pos + glm::vec3(-h.x, -h.y, h.z),
            pos + glm::vec3(h.x, -h.y, h.z),
            pos + glm::vec3(h.x, h.y, h.z),
            pos + glm::vec3(-h.x, h.y, h.z),
        };

        struct FaceDef
        {
            int idx[4];
            glm::vec3 normal;
            glm::vec2 scale;
        };
        FaceDef faces[6] = {
            {{4, 5, 6, 7}, {0, 0, 1}, {size.x, size.y}},  // Front
            {{1, 0, 3, 2}, {0, 0, -1}, {size.x, size.y}}, // Back
            {{0, 4, 7, 3}, {-1, 0, 0}, {size.z, size.y}}, // Left
            {{5, 1, 2, 6}, {1, 0, 0}, {size.z, size.y}},  // Right
            {{3, 7, 6, 2}, {0, 1, 0}, {size.x, size.z}},  // Top
            {{0, 1, 5, 4}, {0, -1, 0}, {size.x, size.z}}, // Bottom
        };

        vector<Vertex> verts;
        vector<u32> indices;
        verts.reserve(24);
        indices.reserve(36);

        glm::vec3 color(1.0f);

        for (auto &f : faces)
        {
            u32 base = static_cast<u32>(verts.size());

            float tileFactor = 0.5f;

            float u_max = tile ? (f.scale.x * tileFactor) : 1.0f;
            float v_max = tile ? (f.scale.y * tileFactor) : 1.0f;

            glm::vec2 uvs[4] = {
                {0, 0},
                {u_max, 0},
                {u_max, v_max},
                {0, v_max}};

            for (int i = 0; i < 4; i++)
            {
                Vertex v{};
                v.position = corners[f.idx[i]];
                v.normal = f.normal;
                v.tex_uv = uvs[i];
                v.color = color;
                verts.push_back(v);
            }

            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 2);
            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 3);
        }

        return Mesh(verts, indices);
    }

    LoadMap Load(const string &filepath)
    {
        LoadMap result;

        std::ifstream file(filepath);
        if (!file)
        {
            std::cerr << "MapLoader: failed to open " << filepath << "\n";
            return result;
        }

        result.blocks.reserve(64);
        result.colliders.reserve(64);
        result.entities.reserve(64);

        std::unordered_map<string, std::shared_ptr<Texture>> textureCache;

        const string textureBasePath = "res/texture/";
        const string defaultTextureName = "Brick01.png";

        string line;
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            std::istringstream ss(line);
            string token;
            ss >> token;

            if (token == "block")
            {
                float x, y, z, sx, sy, sz;
                if (!(ss >> x >> y >> z >> sx >> sy >> sz))
                    continue;

                string textureName = defaultTextureName;
                if (ss >> token)
                    textureName = token;

                int tileFlag = 1;
                ss >> tileFlag;
                bool shouldTile = (tileFlag != 0);

                if (textureCache.find(textureName) == textureCache.end())
                {
                    string fullPath = textureBasePath + textureName;
                    textureCache[textureName] = std::make_shared<Texture>(fullPath);
                }

                glm::vec3 pos(x, y, z);
                glm::vec3 size(sx, sy, sz);

                result.blocks.push_back({pos, size});
                result.colliders.push_back(AABB::fromPosSize(pos, size));

                MapEntity entity;
                entity.mesh = BuildCubeMesh(pos, size, shouldTile);
                entity.texture = textureCache[textureName];
                result.entities.push_back(entity);
            }
            else if (token == "door")
            {
                float x, y, z, sx, sy, sz;
                string req_key, textureName;
                if (!(ss >> x >> y >> z >> sx >> sy >> sz >> req_key >> textureName))
                    continue;

                int tileFlag = 1;
                ss >> tileFlag;
                bool shouldTile = (tileFlag != 0);

                if (textureCache.find(textureName) == textureCache.end())
                {
                    string fullPath = textureBasePath + textureName;
                    textureCache[textureName] = std::make_shared<Texture>(fullPath);
                }

                DoorSpawnData door_data;
                door_data.pos = glm::vec3(x, y, z);
                door_data.size = glm::vec3(sx, sy, sz);
                door_data.req_key = req_key;
                door_data.texture = textureCache[textureName];
                door_data.should_tile = shouldTile;

                result.doors.push_back(door_data);
            }
            else if (token == "enemy")
            {
                float x, y, z;
                string textureName;
                if (!(ss >> x >> y >> z >> textureName))
                    continue;

                if (textureCache.find(textureName) == textureCache.end())
                {
                    string fullPath = textureBasePath + textureName;
                    textureCache[textureName] = std::make_shared<Texture>(fullPath);
                }

                EnemySpawnData enemy_data;
                enemy_data.pos = glm::vec3(x, y, z);
                enemy_data.texture = textureCache[textureName];

                result.enemies.push_back(enemy_data);
            }
            else if (token == "pickup")
            {
                float x, y, z;
                string type_str, item_id, textureName;
                if (!(ss >> x >> y >> z >> type_str >> item_id >> textureName))
                    continue;

                if (textureCache.find(textureName) == textureCache.end())
                {
                    string fullPath = textureBasePath + textureName;
                    textureCache[textureName] = std::make_shared<Texture>(fullPath);
                }

                PickupSpawnData pickup_data;
                pickup_data.pos = glm::vec3(x, y, z);
                pickup_data.type = (type_str == "WEAPON") ? PickupType::WEAPON : PickupType::KEY;
                pickup_data.item_id = item_id;
                pickup_data.texture = textureCache[textureName];

                result.pickups.push_back(pickup_data);
            }
        }

        std::cout << "MapLoader: loaded " << result.blocks.size()
                  << " blocks from " << filepath << "\n";
        return result;
    }
}