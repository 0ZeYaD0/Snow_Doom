#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

using std::string;

enum class KeyCode : int
{
    W = GLFW_KEY_W,
    A = GLFW_KEY_A,
    S = GLFW_KEY_S,
    D = GLFW_KEY_D,
    R = GLFW_KEY_R,
    F = GLFW_KEY_F,
    E = GLFW_KEY_E,
    Space = GLFW_KEY_SPACE,
    Esc = GLFW_KEY_ESCAPE,
    LeftShift = GLFW_KEY_LEFT_SHIFT,
    Tab = GLFW_KEY_TAB
};

enum class MouseCode : int
{
    Left = GLFW_MOUSE_BUTTON_LEFT,
    Right = GLFW_MOUSE_BUTTON_RIGHT,
    Middle = GLFW_MOUSE_BUTTON_MIDDLE
};

enum class InputType
{
    Keyboard,
    Mouse
};

struct ActionBinding
{
    InputType type;
    int code;
};

struct AxisBinding
{
    KeyCode posKey;
    KeyCode negKey;
};

class Input
{
public:
    static void Init(GLFWwindow *window);
    static void Update();

    // Mapper Interface
    static void BindAction(const string &actionName, KeyCode key);
    static void BindAction(const string &actionName, MouseCode button);
    static void BindAxis(const string &axisName, KeyCode posKey, KeyCode negativeKey);

    // High lvl Queries
    static bool GetAction(const string &actionName);
    static bool GetActionDown(const string &actionName);
    static bool GetActionUp(const string &actionName);
    static float GetAxis(const string &axisName);

    // Raw Input Queries
    static bool GetKey(KeyCode key);
    static bool GetKeyDown(KeyCode key);
    static bool GetKeyUp(KeyCode key);

    static bool GetMouseButton(MouseCode button);
    static bool GetMouseButtonDown(MouseCode button);
    static bool GetMouseButtonUp(MouseCode button);

    static glm::vec2 GetMousePosition();
    static glm::vec2 GetMouseDelta();
    static float GetScrollDelta();

    static void ToggleCursor(GLFWwindow *window);
    static void SetCursorHidden(GLFWwindow *window, bool hidden);
    static bool IsCursorHidden();

private:
    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void CursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

    // State arrays
    static bool s_Keys[GLFW_KEY_LAST];
    static bool s_KeysDown[GLFW_KEY_LAST];
    static bool s_KeysUp[GLFW_KEY_LAST];

    static bool s_MouseButtons[GLFW_MOUSE_BUTTON_LAST];
    static bool s_MouseButtonsDown[GLFW_MOUSE_BUTTON_LAST];
    static bool s_MouseButtonsUp[GLFW_MOUSE_BUTTON_LAST];

    // Mapper Dictionaries
    static std::unordered_map<std::string, ActionBinding> s_ActionBindings;
    static std::unordered_map<std::string, AxisBinding> s_AxisBindings;

    // Mouse state
    static glm::vec2 s_MousePosition;
    static glm::vec2 s_MouseDelta;
    static glm::vec2 s_LastMousePosition;
    static bool s_FirstMouse;
    static float s_ScrollDelta;
    static bool s_CursorHidden;
};