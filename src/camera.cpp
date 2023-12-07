#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "glfw_enabled.h"

#include "camera.h"
#include <stdio.h>

std::map<unsigned int, Camera_Movement> inputMap = {
    {GLFW_KEY_W, FORWARD},
    {GLFW_KEY_A, LEFT},
    {GLFW_KEY_S, BACKWARD},
    {GLFW_KEY_D, RIGHT},
    {GLFW_KEY_SPACE, UP},
    {GLFW_KEY_LEFT_CONTROL, DOWN},

};

Camera::Camera(
    glm::vec3 position,
    glm::vec3 up,
    float yaw,
    float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
                   MovementSpeed(SPEED),
                   MouseSensitivity(SENSITIVITY),
                   Zoom(ZOOM)
{
    mPosition = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;

    for (unsigned int i = 0; i < MAX_CAMERA_MOVEMENT; i++)
    {
        mInputs[i] = GLFW_RELEASE;
    }

    updateCameraVectors();
}

Camera::Camera(
    float posX,
    float posY,
    float posZ,
    float upX,
    float upY,
    float upZ,
    float yaw,
    float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
                   MovementSpeed(SPEED),
                   MouseSensitivity(SENSITIVITY),
                   Zoom(ZOOM)
{
    mPosition = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;

    for (unsigned int i = 0; i < MAX_CAMERA_MOVEMENT; i++)
    {
        mInputs[i] = GLFW_RELEASE;
    }

    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(mPosition, mPosition + Front, Up);
}

void Camera::cursorposfun(double xpos, double ypos)
{

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::scrollfun(double xoffset, double yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::update(float dt)
{
    float velocity = MovementSpeed * dt;
    if (mInputs[FORWARD] == GLFW_PRESS)
        mPosition += Front * velocity;
    if (mInputs[BACKWARD] == GLFW_PRESS)
        mPosition -= Front * velocity;
    if (mInputs[LEFT] == GLFW_PRESS)
        mPosition -= Right * velocity;
    if (mInputs[RIGHT] == GLFW_PRESS)
        mPosition += Right * velocity;
    if (mInputs[UP] == GLFW_PRESS)
        mPosition += Up * velocity;
    if (mInputs[DOWN] == GLFW_PRESS)
        mPosition -= Up * velocity;
}

void Camera::keyfun(int key, int scancode, int action, int mods)
{
    if (action == GLFW_REPEAT)
        return;
    std::map<unsigned int, Camera_Movement>::iterator it = inputMap.find(key);
    if (it != inputMap.end())
    {
        mInputs[it->second] = action;
    }
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}
