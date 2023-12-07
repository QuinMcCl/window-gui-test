#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "primitive.h"
#include "window.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

int main()
{

    Window testWindow(10, 10, "you dont see me");
    testWindow.~Window();

    Window MainWindow(SCR_WIDTH, SCR_HEIGHT, "Some Window");
    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader cameraShader("../shaders/camera.vs", "../shaders/camera.fs");
    Shader flatShader("../shaders/flat.vs", "../shaders/flat.fs");

    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

    TextureManager theTextureManager;

    stbi_set_flip_vertically_on_load(true);
    Texture container = theTextureManager.getTexture("../resources/container.jpg", 1);
    Texture wall = theTextureManager.getTexture("../resources/wall.jpg", 1);

    Primitive Cubes[] = {
        {CUBE, &cameraShader, &container},
        {CUBE, &cameraShader, &container},
        {CUBE, &cameraShader, &container},
        {CUBE, &cameraShader, &container},
        {CUBE, &cameraShader, &container},
        {CUBE, &cameraShader, &container},
        {CUBE, &cameraShader, &container},
        {CUBE, &cameraShader, &container},
        {CUBE, &cameraShader, &container},
        {CUBE, &cameraShader, &container},
    };

    for (int i = 0; i < 10; i++)
    {
        MainWindow.adopt(&Cubes[i]);
    }

    Primitive Square(SQUARE, &flatShader, &wall);

    MainWindow.adopt(&Square);

    MainWindow.adopt(&camera);

    // render loop
    // -----------
    while (!MainWindow.shouldClose())
    // while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        MainWindow.update(deltaTime);

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)MainWindow.getWidth() / (float)MainWindow.getHeight(), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        for (unsigned int i = 0; i < 10; i++)
        {
            float angle = 20.0f * i * currentFrame;
            model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
            Cubes[i].updateMatricies(model, view, projection);
        }


        projection = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
        view = glm::mat4(1.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.125f, 0.0125f, 0.0f));
        model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.25f, 0.25f, 1.0f));
        Square.updateMatricies(model, view, projection);

        MainWindow.draw();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
