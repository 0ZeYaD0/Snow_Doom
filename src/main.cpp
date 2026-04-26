#include <core/game.h>

i32 main()
{
    Game *game = new Game();

    game->Run();

    game->Exit();
}

/*
// --- Global State ---
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// --- Shader loading functions ---
static string loadShaderSource(const string &path)
{
    ifstream file(path);
    if (!file)
        throw runtime_error("Failed to open shader file: " + path);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static unsigned int compileShader(GLenum type, const string &source)
{
    unsigned int shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        throw runtime_error("Shader compilation failed");
    }
    return shader;
}

static unsigned int loadShaderFromFile(GLenum type, const string &path)
{
    return compileShader(type, loadShaderSource(path));
}

static unsigned int createShaderProgram(const string &vertexPath, const string &fragmentPath)
{
    unsigned int vertexShader = loadShaderFromFile(GL_VERTEX_SHADER, vertexPath);
    unsigned int fragmentShader = loadShaderFromFile(GL_FRAGMENT_SHADER, fragmentPath);
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
        throw runtime_error("Shader program link failed");
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

int main()
{
    Window win;
    win.SetWindowSize(800, 600);
    win.SetWindowTitle("Input & Camera Test");
    win.InitializeWindow();

    GLFWwindow *window = win.GetWindow();

    // ==========================================
    // 1. INITIALIZE INPUT MANAGER
    // ==========================================
    Input::Init(window);

    unsigned int shaderProgram;
    try
    {
        shaderProgram = createShaderProgram("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");
    }
    catch (const exception &e)
    {
        cout << e.what() << endl;
        return -1;
    }

    // --- Create a simple 3D Cube for testing ---
    float vertices[] = {
        -0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,

        -0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,

        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,

        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,

        -0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        -0.5f,

        -0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        -0.5f,
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
    unsigned int colorLoc = glGetUniformLocation(shaderProgram, "circleColor");

    // --- Jump Physics Variables ---
    float yVelocity = 0.0f;
    float gravity = -15.0f;
    float jumpForce = 10.0f;
    float groundLevel = camera.Position.y; // Assume starting height is the floor
    bool isJumping = false;

    // --- NEW: Box Collider Setup ---
    // Since the vertices above range from -0.5 to 0.5, the cube is 1x1x1 and centered at 0,0,0.
    AABB boxCollider = AABB::fromPosSize(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    while (!win.WindowShouldClose())
    {
        // Calculate deltaTime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ==========================================
        // 2. TOGGLES & SINGLE PRESS EVENTS
        // ==========================================

        // Hide/Show mouse cursor with ESCAPE
        if (Input::GetKeyDown(GLFW_KEY_ESCAPE))
        {
            Input::ToggleCursor(window);
        }

        // Jump logic using SPACEBAR (Only triggers once per press)
        if (Input::GetKeyDown(GLFW_KEY_SPACE) && !isJumping)
        {
            yVelocity = jumpForce;
            isJumping = true;
        }

        // ==========================================
        // 3. CONTINUOUS MOVEMENT & MOUSE LOOK
        // ==========================================

        // Walking
        if (Input::GetKey(GLFW_KEY_W))
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (Input::GetKey(GLFW_KEY_S))
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (Input::GetKey(GLFW_KEY_A))
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (Input::GetKey(GLFW_KEY_D))
            camera.ProcessKeyboard(RIGHT, deltaTime);

        // Looking around (Only works if the cursor is hidden)
        if (Input::IsCursorHidden())
        {
            glm::vec2 mouseDelta = Input::GetMouseDelta();
            // Pass the delta directly to your camera
            camera.ProcessMouseMov(mouseDelta.x, mouseDelta.y);
        }

        // ==========================================
        // 4. APPLY PHYSICS
        // ==========================================
        if (isJumping)
        {
            yVelocity += gravity * deltaTime;           // Apply gravity
            camera.Position.y += yVelocity * deltaTime; // Move camera vertically

            // Ground collision check
            if (camera.Position.y <= groundLevel)
            {
                camera.Position.y = groundLevel;
                isJumping = false;
                yVelocity = 0.0f;
            }
        }

        // --- NEW: Apply AABB Collisions ---
        // This will check the camera's attempted position against the orange cube.
        // If the player walks into it, or jumps and lands on it, they will be pushed out.
        if (Physics::resolveCollision(camera.Position, camera.PlayerSize, boxCollider))
            std::cout << "Collide\n";

        // ==========================================
        // 5. RENDERING
        // ==========================================
        glm::mat4 view = camera.GetViewMat();
        glm::mat4 projection = camera.GetProjectionMat((float)win.GetWidth() / (float)win.GetHeight());

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glUniform4f(colorLoc, 1.0f, 0.5f, 0.2f, 1.0f); // Orange cube
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        Input::Update();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    return 0;
}

*/