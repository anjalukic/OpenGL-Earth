#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <vector>

const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float yaw;
    float pitch;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f)) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), yaw(90.0f), pitch(0.0f)
    {
        Position = position;
    }
    
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
    }

    void ProcessMouseMovement(float xoffset, float yoffset)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        pitch += yoffset;
        yaw += xoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

       
        float radius = 3.0f;
        glm::vec3 pos;
        pos.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        pos.y = sin(glm::radians(pitch));
        pos.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        Position = glm::normalize(pos)* radius;

    }

    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 15.0f)
            Zoom = 15.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }
};
#endif

