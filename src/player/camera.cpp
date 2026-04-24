#include <player/camera.h>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(CameraDefaults::SPEED),
      MouseSens(CameraDefaults::SENS),
      Zoom(CameraDefaults::ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    UpdateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(CameraDefaults::SPEED),
      MouseSens(CameraDefaults::SENS),
      Zoom(CameraDefaults::ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMat() const
{
    return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::GetProjectionMat(float aspectRatio, float nearPlane, float farPlane) const
{
    return glm::perspective(glm::radians(Zoom), aspectRatio, nearPlane, farPlane);
}

void Camera::ProcessKeyboard(CameraMovements direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == CameraMovements::FORWARD)
        Position += Front * velocity;
    if (direction == CameraMovements::BACKWARD)
        Position -= Front * velocity;
    if (direction == CameraMovements::LEFT)
        Position -= Right * velocity;
    if (direction == CameraMovements::RIGHT)
        Position += Right * velocity;
    if (direction == CameraMovements::UP)
        Position += Up * velocity;
    if (direction == CameraMovements::DOWN)
        Position -= Up * velocity;
}

void Camera::ProcessMouseMov(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSens;
    yoffset *= MouseSens;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}