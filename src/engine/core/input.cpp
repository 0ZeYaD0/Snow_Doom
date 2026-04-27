#include <engine/core/input.h>

bool Input::s_Keys[GLFW_KEY_LAST] = {false};
bool Input::s_KeysDown[GLFW_KEY_LAST] = {false};
bool Input::s_KeysUp[GLFW_KEY_LAST] = {false};

bool Input::s_MouseButtons[GLFW_MOUSE_BUTTON_LAST] = {false};
bool Input::s_MouseButtonsDown[GLFW_MOUSE_BUTTON_LAST] = {false};
bool Input::s_MouseButtonsUp[GLFW_MOUSE_BUTTON_LAST] = {false};

glm::vec2 Input::s_MousePosition = glm::vec2(0.0f);
glm::vec2 Input::s_MouseDelta = glm::vec2(0.0f);
glm::vec2 Input::s_LastMousePosition = glm::vec2(0.0f);
bool Input::s_FirstMouse = true;

float Input::s_ScrollDelta = 0.0f;
bool Input::s_CursorHidden = false;

void Input::Init(GLFWwindow *window)
{
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    SetCursorHidden(window, true);
}

void Input::Update()
{
    for (int i = 0; i < GLFW_KEY_LAST; i++)
    {
        s_KeysDown[i] = false;
        s_KeysUp[i] = false;
    }

    for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++)
    {
        s_MouseButtonsDown[i] = false;
        s_MouseButtonsUp[i] = false;
    }

    s_MouseDelta = glm::vec2(0.0f);
    s_ScrollDelta = 0.0f;
}

bool Input::GetKey(int key) { return key >= 0 && key < GLFW_KEY_LAST ? s_Keys[key] : false; }
bool Input::GetKeyDown(int key) { return key >= 0 && key < GLFW_KEY_LAST ? s_KeysDown[key] : false; }
bool Input::GetKeyUp(int key) { return key >= 0 && key < GLFW_KEY_LAST ? s_KeysUp[key] : false; }

bool Input::GetMouseButton(int button) { return button >= 0 && button < GLFW_MOUSE_BUTTON_LAST ? s_MouseButtons[button] : false; }
bool Input::GetMouseButtonDown(int button) { return button >= 0 && button < GLFW_MOUSE_BUTTON_LAST ? s_MouseButtonsDown[button] : false; }
bool Input::GetMouseButtonUp(int button) { return button >= 0 && button < GLFW_MOUSE_BUTTON_LAST ? s_MouseButtonsUp[button] : false; }

glm::vec2 Input::GetMousePosition() { return s_MousePosition; }
glm::vec2 Input::GetMouseDelta() { return s_MouseDelta; }
float Input::GetScrollDelta() { return s_ScrollDelta; }
bool Input::IsCursorHidden() { return s_CursorHidden; }

void Input::ToggleCursor(GLFWwindow *window)
{
    SetCursorHidden(window, !s_CursorHidden);
}

void Input::SetCursorHidden(GLFWwindow *window, bool hidden)
{
    s_CursorHidden = hidden;
    if (hidden)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        s_FirstMouse = true; // Prevent camera snap when re-hiding cursor
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Input::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key >= 0 && key < GLFW_KEY_LAST)
    {
        if (action == GLFW_PRESS)
        {
            s_Keys[key] = true;
            s_KeysDown[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            s_Keys[key] = false;
            s_KeysUp[key] = true;
        }
    }
}

void Input::CursorPositionCallback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (s_FirstMouse)
    {
        s_LastMousePosition = glm::vec2(xpos, ypos);
        s_FirstMouse = false;
    }

    s_MouseDelta.x = xpos - s_LastMousePosition.x;
    s_MouseDelta.y = s_LastMousePosition.y - ypos; // Reversed since y-coords go from bottom to top

    s_LastMousePosition = glm::vec2(xpos, ypos);
    s_MousePosition = glm::vec2(xpos, ypos);
}

void Input::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button >= 0 && button < GLFW_MOUSE_BUTTON_LAST)
    {
        if (action == GLFW_PRESS)
        {
            s_MouseButtons[button] = true;
            s_MouseButtonsDown[button] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            s_MouseButtons[button] = false;
            s_MouseButtonsUp[button] = true;
        }
    }
}

void Input::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    s_ScrollDelta = static_cast<float>(yoffset);
}