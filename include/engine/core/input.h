#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Input
{
public:
    static void Init(GLFWwindow *window);

    static void Update();

    static bool GetKey(int key);
    static bool GetKeyDown(int key);
    static bool GetKeyUp(int key);

    static bool GetMouseButton(int button);
    static bool GetMouseButtonDown(int button);
    static bool GetMouseButtonUp(int button);

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

    static bool s_Keys[GLFW_KEY_LAST];
    static bool s_KeysDown[GLFW_KEY_LAST];
    static bool s_KeysUp[GLFW_KEY_LAST];

    static bool s_MouseButtons[GLFW_MOUSE_BUTTON_LAST];
    static bool s_MouseButtonsDown[GLFW_MOUSE_BUTTON_LAST];
    static bool s_MouseButtonsUp[GLFW_MOUSE_BUTTON_LAST];

    static glm::vec2 s_MousePosition;
    static glm::vec2 s_MouseDelta;
    static glm::vec2 s_LastMousePosition;
    static bool s_FirstMouse;

    static float s_ScrollDelta;
    static bool s_CursorHidden;
};