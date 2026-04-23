#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>

using namespace std;

std::string loadShader(const char *path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void drawEllipse(unsigned int shaderProgram, float cx, float cy, float rx, float ry, int num_segments, float start_angle, float end_angle, bool is_filled, float red, float green, float blue, float alpha = 1.0f)
{
    std::vector<float> vertices;

    if (is_filled)
    {
        vertices.push_back(cx);
        vertices.push_back(cy);
        vertices.push_back(0.0f);
    }

    float angle_range = end_angle - start_angle;
    float theta = angle_range / float(num_segments);

    for (int i = 0; i <= num_segments; ++i)
    {
        float angle = start_angle + i * theta;

        // UPDATED: Using rx for the X axis and ry for the Y axis
        float x = rx * cosf(angle) + cx;
        float y = ry * sinf(angle) + cy;

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
    }

    // ... [The rest of the VAO, VBO, Color, and Draw code remains EXACTLY the same!] ...

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);
    int colorLocation = glGetUniformLocation(shaderProgram, "circleColor");
    glUniform4f(colorLocation, red, green, blue, alpha);

    if (is_filled)
    {
        glDrawArrays(GL_TRIANGLE_FAN, 0, num_segments + 2);
    }
    else
    {
        if (std::abs(angle_range - (2.0f * 3.14159265359f)) < 0.01f)
            glDrawArrays(GL_LINE_LOOP, 0, num_segments + 1);
        else
            glDrawArrays(GL_LINE_STRIP, 0, num_segments + 1);
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

int main()
{
    if (!glfwInit())
    {
        cout << "Failed to initialize GLFW" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window;
    window = glfwCreateWindow(800, 600, "test", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to open GLFW window" << endl;
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    std::string vertexCode = loadShader("vertex.glsl");
    const char *vertexShaderSource = vertexCode.c_str();
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    std::string fragmentCode = loadShader("fragment.glsl");
    const char *fragmentShaderSource = fragmentCode.c_str();
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // 1. THE LEGS
        drawEllipse(shaderProgram, -0.30f, -0.05f, 0.1f, 0.2f, 36, 0.0f, -1.0f * 3.14159f, true, 1.0f, 0.0f, 1.0f); // Left Leg
        drawEllipse(shaderProgram, 0.00f, -0.05f, 0.1f, 0.2f, 36, 0.0f, -1.0f * 3.14159f, true, 1.0f, 0.0f, 1.0f);  // Middle Leg (Slightly lower)
        drawEllipse(shaderProgram, 0.30f, -0.05f, 0.1f, 0.2f, 36, 0.0f, -1.0f * 3.14159f, true, 1.0f, 0.0f, 1.0f);  // Right Leg

        // 2. THE MAIN HEAD
        drawEllipse(shaderProgram, 0.0f, -0.05f, 0.4f, 0.4f, 36, 0.0f, 3.14159f, true, 1.0f, 0.0f, 1.0f);

        // 3. THE EYE WHITES (2 Tall White Ellipses)
        drawEllipse(shaderProgram, -0.15f, 0.15f, 0.08f, 0.15f, 36, 0.0f, 2.0f * 3.14159f, true, 1.0f, 1.0f, 1.0f); // Left White
        drawEllipse(shaderProgram, 0.15f, 0.15f, 0.08f, 0.15f, 36, 0.0f, 2.0f * 3.14159f, true, 1.0f, 1.0f, 1.0f);  // Right White

        // 4. THE PUPILS
        drawEllipse(shaderProgram, -0.13f, 0.13f, 0.04f, 0.04f, 36, 0.0f, 2.0f * 3.14159f, true, 0.0f, 0.0f, 0.0f); // Left Pupil
        drawEllipse(shaderProgram, 0.17f, 0.13f, 0.04f, 0.04f, 36, 0.0f, 2.0f * 3.14159f, true, 0.0f, 0.0f, 0.0f);  // Right Pupil

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
