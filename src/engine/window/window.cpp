#include <engine/window/window.h>

#include <iostream>

i32 Window::width = DEF_WIDTH, Window::height = DEF_HEIGHT;

void Window::ErrorCallback(i32 error, const char *description)
{
    fprintf(stderr, "error: %s\n", description);
}

void OpenGLErrorCallback(u32 source, u32 type, u32 id, u32 severity, i32 length, const char *msg, const void *userParam)
{
    return;
}

void Window::OnWindowResize(GLFWwindow *window, i32 width, i32 height)
{
    glViewport(0, 0, width, height);
    Window::SetWindowSize(width, height);
}

void Window::InitializeWindow()
{
    // initialize glfw
    if (!glfwInit())
    {
        ErrorCallback(1, "failed to init GLFW.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VRS_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VRS_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    glfwSetErrorCallback(ErrorCallback);

    Window::monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *screen = glfwGetVideoMode(monitor);
    Window::monitor_width = screen->width;
    Window::monitor_height = screen->width;

    if (fullscreen)
    {
        SetWindowSize(monitor_width, monitor_height);

        window = glfwCreateWindow(
            monitor_width, monitor_height,
            GetTitle().c_str(),
            monitor, nullptr);
    }
    else
    {
        window = glfwCreateWindow(
            GetWidth(), GetHeight(),
            GetTitle().c_str(),
            nullptr, nullptr);
    }

    if (!window)
    {
        ErrorCallback(1, "window was not correctly initialized");
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        ErrorCallback(1, "couldn't load OpenGL");
    }

    std::cout << "OpenGL version: " << GLVersion.major << "." << GLVersion.minor << "\n";

    // enable openGL error callback
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(OpenGLErrorCallback, 0);

    if (VSyncEnabled())
        glfwSwapInterval(1);

    if (resizeable)
    {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwSetFramebufferSizeCallback(window, OnWindowResize);
    }

    // input callbacks
    // glfwSetKeyCallback(window, Keyboard::KeyCallback);

    // glfwSetCursorPosCallback(window, Mouse::CursorPosCallback);
    // glfwSetMouseButtonCallback(window, Mouse::MouseButtonCallback);
    // glfwSetScrollCallback(window, Mouse::MouseWheelCallback);

    // // joystick initialization
    // mainJoystick.Update();

    // if(mainJoystick.IsPresent())
    //     Log(LOG_INFO) << "joystick " << mainJoystick.GetName() << " is present\n";
    // else
    //     Log(LOG_INFO) << "no joystick found.\n";
}

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}