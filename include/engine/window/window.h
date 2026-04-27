#pragma once

#include <engine/core/defines.h>

#include <string>
using std::string;

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define DEF_HEIGHT 480
#define DEF_WIDTH  640

#define GL_VRS_MAJOR 4
#define GL_VRS_MINOR 6

class Window
{
private:
    static i32 width, height;
    i32 monitor_width, monitor_height;
    bool fullscreen, resizeable, vsync;

    string title = "GLFW window";

protected:
    GLFWwindow *window;
    GLFWmonitor *monitor;

public:
    Window()
        :fullscreen{false}, resizeable{true},
         vsync{true}, title{"GLFW window"},
         monitor_width{DEF_WIDTH}, monitor_height{DEF_HEIGHT}
    {
        width = DEF_WIDTH;
        height = DEF_HEIGHT;
    }

    // --- getters
    inline i32 GetWidth() const { return Window::width; }
    inline i32 GetHeight() const { return Window::height; }

    inline string GetTitle() const { return this->title; }

    inline GLFWwindow *GetWindow() const { return this->window; }
    inline GLFWmonitor *GetMonitor() const { return this->monitor; }

    inline bool VSyncEnabled() const { return this->vsync; }


    // --- setters
    static inline void SetWindowSize(i32 width, i32 height)
    {
        if(width <= 0 || height <= 0) {
            SetDefaultWindowSize();
            return;
        }

        Window::width = width;
        Window::height = height;
    }

    static inline void SetDefaultWindowSize()
    {
        width = DEF_WIDTH;
        height = DEF_HEIGHT;
    }

    inline void SetFullScreen(bool fs) { this->fullscreen = fs; }
    inline void SetResizeable(bool rs) { this->resizeable = true; }
    inline void SetVSync(bool vs) { this->vsync = vs; }
    inline void SetWindowTitle(string title) { this->title = title; }


    // --- initialization
    void InitializeWindow();
    
    // main game loop
    inline bool WindowShouldClose() { return glfwWindowShouldClose(window); }
    inline void SetWindowShouldClose(bool shouldClose) { glfwSetWindowShouldClose(window, shouldClose); }

    static void ErrorCallback(i32 error, const char *description);
    static void OnWindowResize(GLFWwindow *window, i32 width, i32 height);

    // extras
    inline void EnableCursor() { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
    inline void DisableCursor() { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }

    // --- destructor
    ~Window();
};