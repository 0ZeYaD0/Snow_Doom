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

// Mapper Dict INIT
std::unordered_map<std::string, ActionBinding> Input::s_ActionBindings;
std::unordered_map<std::string, AxisBinding> Input::s_AxisBindings;

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

void Input::BindAction(const string &actionName, KeyCode key)
{
    s_ActionBindings[actionName] = {InputType::Keyboard, static_cast<int>(key)};
}

void Input::BindAction(const string &actionName, MouseCode button)
{
    s_ActionBindings[actionName] = {InputType::Mouse, static_cast<int>(button)};
}

void Input::BindAxis(const string &axisName, KeyCode posKey, KeyCode negKey)
{
    s_AxisBindings[axisName] = {posKey, negKey};
}
// --- High Level Queries ---
bool Input::GetAction(const string &actionName)
{
    auto it = s_ActionBindings.find(actionName);
    if (it != s_ActionBindings.end())
    {
        if (it->second.type == InputType::Keyboard)
            return s_Keys[it->second.code];
        else
            return s_MouseButtons[it->second.code];
    }
    return false;
}

bool Input::GetActionDown(const string &actionName)
{
    auto it = s_ActionBindings.find(actionName);
    if (it != s_ActionBindings.end())
    {
        if (it->second.type == InputType::Keyboard)
            return s_KeysDown[it->second.code];
        else
            return s_MouseButtonsDown[it->second.code];
    }
    return false;
}

bool Input::GetActionUp(const string &actionName)
{
    auto it = s_ActionBindings.find(actionName);
    if (it != s_ActionBindings.end())
    {
        if (it->second.type == InputType::Keyboard)
            return s_KeysUp[it->second.code];
        else
            return s_MouseButtonsUp[it->second.code];
    }
    return false;
}

float Input::GetAxis(const string &axisName)
{
    auto it = s_AxisBindings.find(axisName);
    if (it != s_AxisBindings.end())
    {
        float val = 0.0f;
        if (s_Keys[static_cast<int>(it->second.posKey)])
            val += 1.0f;
        if (s_Keys[static_cast<int>(it->second.negKey)])
            val -= 1.0f;
        return val;
    }
    return 0.0f;
}

// --- Raw Input Queries ---
bool Input::GetKey(KeyCode key) { return s_Keys[static_cast<int>(key)]; }
bool Input::GetKeyDown(KeyCode key) { return s_KeysDown[static_cast<int>(key)]; }
bool Input::GetKeyUp(KeyCode key) { return s_KeysUp[static_cast<int>(key)]; }

bool Input::GetMouseButton(MouseCode button) { return s_MouseButtons[static_cast<int>(button)]; }
bool Input::GetMouseButtonDown(MouseCode button) { return s_MouseButtonsDown[static_cast<int>(button)]; }
bool Input::GetMouseButtonUp(MouseCode button) { return s_MouseButtonsUp[static_cast<int>(button)]; }

glm::vec2 Input::GetMousePosition() { return s_MousePosition; }
glm::vec2 Input::GetMouseDelta() { return s_MouseDelta; }
float Input::GetScrollDelta() { return s_ScrollDelta; }

void Input::ToggleCursor(GLFWwindow *window)
{
    s_CursorHidden = !s_CursorHidden;
    glfwSetInputMode(window, GLFW_CURSOR, s_CursorHidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Input::SetCursorHidden(GLFWwindow *window, bool hidden)
{
    s_CursorHidden = hidden;
    glfwSetInputMode(window, GLFW_CURSOR, s_CursorHidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

bool Input::IsCursorHidden() { return s_CursorHidden; }

// --- Hardware Callbacks ---
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
    s_MouseDelta.y = s_LastMousePosition.y - ypos;

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