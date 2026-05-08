#include <engine/map/map_loader.h>

#include <fstream>
#include <sstream>
#include <iostream>

namespace MapLoader
{
    static Mesh BuildCubeMesh(glm::vec3 pos, glm::vec3 size)
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
        };
        FaceDef faces[6] = {
            {{4, 5, 6, 7}, {0, 0, 1}},
            {{1, 0, 3, 2}, {0, 0, -1}},
            {{0, 4, 7, 3}, {-1, 0, 0}},
            {{5, 1, 2, 6}, {1, 0, 0}},
            {{3, 7, 6, 2}, {0, 1, 0}},
            {{0, 1, 5, 4}, {0, -1, 0}},
        };

        vector<Vertex> verts;
        vector<u32> indices;
        verts.reserve(24);
        indices.reserve(36);

        glm::vec2 uvs[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
        glm::vec3 color(0.8f);

        for (auto &f : faces)
        {
            u32 base = static_cast<u32>(verts.size());

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
        result.meshes.reserve(64);

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
                {
                    std::cerr << "MapLoader: bad block line: " << line << "\n";
                    continue;
                }

                glm::vec3 pos(x, y, z);
                glm::vec3 size(sx, sy, sz);

                result.blocks.push_back({pos, size});
                result.colliders.push_back(AABB::fromPosSize(pos, size));
                result.meshes.push_back(BuildCubeMesh(pos, size));
            }
        }

        std::cout << "MapLoader: loaded " << result.blocks.size()
                  << " blocks from " << filepath << "\n";
        return result;
    }
}