#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader/tiny_obj_loader.h>
#include <graphics/object_loader.h>

#include <iostream>

namespace objloader
{
    vector<Mesh> LoadModel(const string &filepath)
    {
        vector<Mesh> laoded_meshes;

        tinyobj::attrib_t attrib;
        vector<tinyobj::shape_t> shapes;
        vector<tinyobj::material_t> materials;
        string err;

        bool success = tinyobj::LoadObj(
            &attrib, &shapes, &materials, &err, filepath.c_str()
        );

        if(!err.empty()) {
            std::cerr << "OBJ ERROR: " << err << std::endl;
        }

        if(!success) {
            return laoded_meshes;
        }

        for(const auto &shape : shapes) {
            vector<Vertex> vertices;
            vector<u32> indices;

            u64 index_offset = 0;

            for(u64 f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                u64 fv = u64(shape.mesh.num_face_vertices[f]);

                for(u64 v = 0; v < fv; v++) {
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                    Vertex vertex{};

                    vertex.position = {
                        attrib.vertices[3 * u64(idx.vertex_index) + 0],
                        attrib.vertices[3 * u64(idx.vertex_index) + 1],
                        attrib.vertices[3 * u64(idx.vertex_index) + 2]
                    };

                    if(idx.normal_index >= 0) {
                        vertex.normal = {
                            attrib.normals[3 * u64(idx.normal_index) + 0],
                            attrib.normals[3 * u64(idx.normal_index) + 1],
                            attrib.normals[3 * u64(idx.normal_index) + 2]
                        };
                    }

                    if(idx.texcoord_index >= 0) {
                        vertex.tex_uv = {
                            attrib.texcoords[2 * u64(idx.texcoord_index) + 0],
                            attrib.texcoords[2 * u64(idx.texcoord_index) + 1],
                        };
                    }

                    vertices.push_back(vertex);
                    indices.push_back(static_cast<u32>(indices.size()));
                }
                index_offset += fv;
            }

            laoded_meshes.emplace_back(vertices, indices);
        }

        return laoded_meshes;
    }
}