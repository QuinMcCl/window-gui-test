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

bool Camera::handleEvent(GLFW_EVENT event)
{
    bool stop = false;
    switch (event.Type)
    {
    case CURSORPOS:
        stop |= cursorposfun(event.mCursorPos);
        break;
    case MOUSEBUTTON:
        stop |= mousebuttonfun(event.mMouseButton);
        break;
    case SCROLL:
        stop |= scrollfun(event.mScroll);
        break;
    case KEY:
        stop |= keyfun(event.mKey);
        break;
    default:
        break;
    }

    if (!stop)
        stop |= glfw_enabled::handleEvent(event);

    return stop;
};

bool Camera::cursorposfun(CURSORPOS_EVENT event)
{
    bool stop = false;
    if (firstMouse)
    {
        lastX = event.xpos;
        lastY = event.ypos;
        firstMouse = false;
    }

    float xoffset = event.xpos - lastX;
    float yoffset = lastY - event.ypos; // reversed since y-coordinates go from bottom to top

    lastX = event.xpos;
    lastY = event.ypos;

    if (mLeftMouseDown == GLFW_PRESS)
    {

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
    return stop;
}

bool Camera::mousebuttonfun(MOUSEBUTTON_EVENT event)
{
    bool stop = false;
    if (event.action == GLFW_REPEAT)
        return stop;
    if (event.button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (event.action == GLFW_PRESS)
        {
            mLeftMouseDown = GLFW_PRESS;
            mLeftDownPosX = lastX;
            mLeftDownPosY = lastY;
        }
        else if (event.action == GLFW_RELEASE)
        {
            mLeftMouseDown = GLFW_RELEASE;
        }
    }
    return stop;
}

bool Camera::scrollfun(SCROLL_EVENT event)
{
    bool stop = false;
    Zoom -= (float)event.yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
    return stop;
}

bool Camera::keyfun(KEY_EVENT event)
{
    bool stop = false;
    if (event.action == GLFW_REPEAT)
        return stop;
    std::map<unsigned int, Camera_Movement>::iterator it = inputMap.find(event.key);
    if (it != inputMap.end())
    {
        mInputs[it->second] = event.action;
    }
    return stop;
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
