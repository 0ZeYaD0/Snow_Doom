#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovements
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

namespace CameraDefaults
{
    constexpr float YAW = -90.0f;
    constexpr float PITCH = 0.0f;
    constexpr float SPEED = 2.5;
    constexpr float SENS = 0.1f;
    constexpr float ZOOM = 45.0F;
}

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSens;
    float Zoom;

public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = CameraDefaults::YAW,
           float pitch = CameraDefaults::PITCH);

    Camera(float posX, float posY, float posZ,
           float upX, float upY, float upZ,
           float yaw, float pitch);

    glm::mat4 GetViewMat() const;

    glm::mat4 GetProjectionMat(float aspectRatio, float nearPlane = 0.1f, float farPlane = 100.0f) const;

    void ProcessKeyboard(CameraMovements dir, float deltaTime);

    void ProcessMouseMov(float xoffset, float yoffset, bool constrain = true);

private:
    void UpdateCameraVectors();
};