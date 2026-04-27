#pragma once

#include <engine/core/defines.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <engine/graphics/mesh.h>
#include <engine/graphics/shader.h>

class Transform
{
public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); // euler angles
    glm::vec3 scale    = glm::vec3(1.0f);

    glm::mat4 GetModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, position);

        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        model = glm::scale(model, scale);

        return model;
    }
};

class Entity
{
public:
    Transform transform;
    Mesh *mesh;

    Entity(Mesh *mesh) : mesh(mesh) {}

    virtual void Update(f32 delta_time) {}

    virtual void Draw(const Shader &shader) const
    {
        shader.Use();
        shader.SetMat4("model", transform.GetModelMatrix());

        if(mesh) {
            mesh->Draw(shader);
        }
    }
};