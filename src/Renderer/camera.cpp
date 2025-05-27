#include "camera.h"
#include <glm/gtx/string_cast.hpp> // for glm::to_string if needed in logging

camera::camera(float height, float width, glm::vec3 position)
    : m_h(height), m_w(width), m_position(position) 
{
    LOG(LogLevel::_INFO, "Camera initialized at position: " + glm::to_string(m_position));
}

void camera::inputs(GLFWwindow* window, float& speed, const float& sensitivity) 
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_position += speed * front;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_position -= speed * front;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_position -= speed * glm::normalize(glm::cross(front, up));

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_position += speed * glm::normalize(glm::cross(front, up));

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        m_position -= speed * up;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        m_position += speed * up;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if (!firstclick) {
            LOG(LogLevel::_INFO, "Camera control disabled. Cursor released.");
            firstclick = true;
        }
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (firstclick) {
            glfwSetCursorPos(window, lastx, lasty);
            LOG(LogLevel::_INFO, "Camera control enabled. Cursor locked.");
            firstclick = false;
        }
    }

    if (!firstclick) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        yaw = sensitivity * ((ypos - lasty) / 2.0f) / m_h;
        pitch = sensitivity * ((xpos - lastx) / 2.0f) / m_w;
        lastx = xpos;
        lasty = ypos;

        newfront = glm::rotate(front, glm::radians(-yaw), glm::normalize(glm::cross(front, up)));

        if (!(glm::angle(newfront, up) <= glm::radians(5.0f) || glm::angle(newfront, -up) <= glm::radians(5.0f)))
            front = newfront;
        else
            LOG(LogLevel::_WARNING, "Camera pitch limit reached. Rotation blocked.");

        front = glm::rotate(front, glm::radians(pitch), -up);
    }

    glfwGetWindowSize(window, &m_h, &m_w);
    View = glm::lookAt(m_position, m_position + front, up);

    if (m_h != 0 && m_w != 0) {
        Projection = glm::perspective(glm::radians(45.0f), static_cast<float>(m_h) / static_cast<float>(m_w), 0.1f, 100.0f);
    } else {
        Projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f,1000.0f);
        LOG(LogLevel::_WARNING, "Window size invalid. Default aspect ratio used.");
    }

    Campos = m_position;
}

glm::vec3 camera::getPosition() { return m_position; }
glm::vec3 camera::getFront() { return front; }

const glm::mat4 camera::GetViewMatrix() { return View; }
const glm::mat4 camera::GetProjectionMatrix() { return Projection; }
const glm::vec3 camera::GetCameraPos() { return Campos; }
