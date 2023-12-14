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
#include "texture.h"
#include "window.h"
#include "camera.h"
#include "primitive.h"
#include "instancedPrimitive.h"
#include "fonts.h"

#include <iostream>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

int main()
{

    Window testWindow(10, 10, "you dont see me");
    testWindow.cleanup();

    Window MainWindow(SCR_WIDTH, SCR_HEIGHT, "Some Window");
    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    uniformBufferedObject cameraViewProjection = uniformBufferedObject("uboViewProjection", 2 * sizeof(glm::mat4), GL_STREAM_DRAW);
    uniformBufferedObject screenViewProjection = uniformBufferedObject("uboViewProjection", 2 * sizeof(glm::mat4), GL_STREAM_DRAW);

    glm::mat4 view_projection[2];

    view_projection[0] = camera.GetViewMatrix();
    view_projection[1] = glm::perspective(glm::radians(camera.Zoom), (float)MainWindow.getWidth() / (float)MainWindow.getHeight(), 0.1f, 100.0f);

    cameraViewProjection.fill(view_projection, 2 * sizeof(glm::mat4), 0);

    view_projection[0] = glm::mat4(1.0f);
    view_projection[1] = glm::ortho(0.0f, (float)MainWindow.getWidth(), (float)MainWindow.getHeight(), 0.0f, -1.0f, 1.0f);

    screenViewProjection.fill(view_projection, 2 * sizeof(glm::mat4), 0);

    // build and compile our shader program
    // ------------------------------------
    Shader InstancedCameraShader("../shaders/instanced_camera.vs", "../shaders/instanced_camera.fs");
    Shader cameraFontShader("../shaders/font.vs", "../shaders/font.fs");
    Shader screenFontShader("../shaders/font.vs", "../shaders/font.fs");

    TextureManager theTextureManager;

    stbi_set_flip_vertically_on_load(true);
    Texture container = theTextureManager.getTexture("../resources/container.jpg");
    Texture wall = theTextureManager.getTexture("../resources/wall.jpg");
    Texture ScriptAtlas = theTextureManager.getTexture("../fonts/scripts.png");
    Texture ArialAtlas = theTextureManager.getTexture("../fonts/ArialFontAtlas.png");

    FontType Script = FontType("../fonts/scripts.csv", &ScriptAtlas);
    FontType Arial = FontType("../fonts/ArialFontAtlasMeta.csv", &ArialAtlas);

    RenderedText ScriptCameraRenderer(&Script, &cameraFontShader);
    RenderedText ArialScreenRenderer(&Arial, &screenFontShader);

    InstancedPrimitive Cubes(CUBE, &InstancedCameraShader, &container);

    InstancedCameraShader.use();
    InstancedCameraShader.bindBuffer(cameraViewProjection.getName().c_str(), cameraViewProjection.getBlockBindingIndex());

    cameraFontShader.use();
    cameraFontShader.bindBuffer(cameraViewProjection.getName().c_str(), cameraViewProjection.getBlockBindingIndex());

    screenFontShader.use();
    screenFontShader.bindBuffer(screenViewProjection.getName().c_str(), screenViewProjection.getBlockBindingIndex());

    MainWindow.adopt(&camera);
    MainWindow.adopt(&Cubes);
    MainWindow.adopt(&ArialScreenRenderer);
    MainWindow.adopt(&ScriptCameraRenderer);

    glm::mat4 models[] = {glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f)};
    glm::mat4 model = glm::mat4(1.0f);
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

    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(160.0f, 160.0f, 1.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.5f));
    ArialScreenRenderer.updateModel(model);

    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.0f, -1.0f, 1.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.5f));
    ScriptCameraRenderer.updateModel(model);

    ArialScreenRenderer.setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    ArialScreenRenderer.setText("This is a test");
    ScriptCameraRenderer.setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    ScriptCameraRenderer.setText("abcdefghijklmnopqrstuvwxyz\n1234567890-=[];',./\\\nABCDEFGHIJKLMNOPQRSTUVWXYZ\n!@#$%^&*()_+{}:\"<>?|");

    // render loop
    // -----------
    while (!MainWindow.shouldClose())
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        MainWindow.update(deltaTime);

        glm::mat4 view_projection[2];

        view_projection[0] = camera.GetViewMatrix();
        view_projection[1] = glm::perspective(glm::radians(camera.Zoom), (float)MainWindow.getWidth() / (float)MainWindow.getHeight(), 0.1f, 100.0f);

        cameraViewProjection.fill(view_projection, 2 * sizeof(glm::mat4), 0);

        view_projection[0] = glm::mat4(1.0f);
        view_projection[1] = glm::ortho(0.0f, (float)MainWindow.getWidth(), (float)MainWindow.getHeight(), 0.0f, -1.0f, 1.0f);

        screenViewProjection.fill(view_projection, 2 * sizeof(glm::mat4), 0);

        for (unsigned int i = 0; i < 10; i++)
        {
            float angle = 20.0f * i * currentFrame;
            models[i] = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            models[i] = glm::translate(models[i], glm::vec3(glm::sin(glm::radians(20.0f * i * currentFrame)), glm::cos(glm::radians(20.0f * i * currentFrame)), 0.0f));
            models[i] = glm::translate(models[i], cubePositions[i]);
            models[i] = glm::rotate(models[i], glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            models[i] = glm::scale(models[i], glm::vec3(0.5f, 0.5f, 0.5f));
            models[i] = glm::translate(models[i], glm::vec3(-1.0f, -1.0f, -1.0f));
        }
        Cubes.updateModels(10, models);

        ArialScreenRenderer.setColor(glm::vec4(
            glm::sin(glm::radians(20.0f * currentFrame)), 
            glm::sin(glm::radians(40.0f * currentFrame)), 
            glm::sin(glm::radians(60.0f * currentFrame)), 
            1.0f
            ));

        MainWindow.draw();
    }

    theTextureManager.cleanup();
    Script.cleanup();
    Arial.cleanup();
    InstancedCameraShader.cleanup();
    cameraFontShader.cleanup();
    screenFontShader.cleanup();
    screenViewProjection.cleanup();
    cameraViewProjection.cleanup();
    MainWindow.cleanup();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
