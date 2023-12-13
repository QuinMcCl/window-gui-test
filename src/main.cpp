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

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

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

    // build and compile our shader program
    // ------------------------------------
    Shader cameraShader("../shaders/camera.vs", "../shaders/camera.fs");
    Shader InstancedCameraShader("../shaders/instanced_camera.vs", "../shaders/instanced_camera.fs");
    Shader flatShader("../shaders/flat.vs", "../shaders/flat.fs");
    Shader instancedFlatShader("../shaders/instanced_flat.vs", "../shaders/instanced_flat.fs");
    Shader fontShader("../shaders/font.vs", "../shaders/font.fs");

    TextureManager theTextureManager;

    stbi_set_flip_vertically_on_load(true);
    Texture container = theTextureManager.getTexture("../resources/container.jpg");
    Texture wall = theTextureManager.getTexture("../resources/wall.jpg");
    Texture ArialAtlas = theTextureManager.getTexture("../fonts/ArialFontAtlas.png");
    Texture ScriptAtlas = theTextureManager.getTexture("../fonts/scripts.png");

    FontType Arial = FontType("../fonts/ArialFontAtlasMeta.csv", &ArialAtlas);
    FontType Script = FontType("../fonts/scripts.csv", &ScriptAtlas);

    RenderedText ArialRenderer(&Arial, &fontShader);
    RenderedText ScriptRenderer(&Script, &fontShader);

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

    InstancedPrimitive Cubes(CUBE, &InstancedCameraShader, &container);
    // Primitive Square(SQUARE, &flatShader, &wall);

    MainWindow.adopt(&camera);
    MainWindow.adopt(&Cubes);
    // MainWindow.adopt(&Square);
    MainWindow.adopt(&ArialRenderer);
    MainWindow.adopt(&ScriptRenderer);

    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 models[] = {glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f)};
    glm::mat4 model = glm::mat4(1.0f);

    
    projection = glm::ortho(0.0f, (float)MainWindow.getWidth(), (float)MainWindow.getHeight(), 0.0f, -1.0f, 1.0f);
    view = glm::mat4(1.0f);
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(160.0f, 160.0f, 1.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.5f));
    ArialRenderer.updateMatricies(model, view, projection);
    ArialRenderer.setColorText(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "This is a test");
    ScriptRenderer.setColorText(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "abcdefghijklmnopqrstuvwxyz\n1234567890-=[];',./\\\nABCDEFGHIJKLMNOPQRSTUVWXYZ\n!@#$%^&*()_+{}:\"<>?|");


    // projection = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
    // view = glm::mat4(1.0f);
    // model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(-0.125f, 0.0125f, 0.0f));
    // model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // model = glm::scale(model, glm::vec3(0.25f, 0.25f, 1.0f));
    // Square.updateMatricies(model, view, projection);

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

        projection = glm::perspective(glm::radians(camera.Zoom), (float)MainWindow.getWidth() / (float)MainWindow.getHeight(), 0.1f, 100.0f);
        view = camera.GetViewMatrix();
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
        Cubes.updateMatricies(10, models, view, projection);

        // projection = glm::ortho(0.0f, (float)MainWindow.getWidth(), (float)MainWindow.getHeight(), 0.0f, -1.0f, 1.0f);
        // view = glm::mat4(1.0f);
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f, -1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.5f));
        ScriptRenderer.updateMatricies(model, view, projection);
        

        MainWindow.draw();
    }

    MainWindow.cleanup();
    theTextureManager.cleanup();
    cameraShader.cleanup();
    InstancedCameraShader.cleanup();
    flatShader.cleanup();
    instancedFlatShader.cleanup();
    fontShader.cleanup();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
