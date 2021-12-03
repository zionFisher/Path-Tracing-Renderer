#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Global.hpp"

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Right;

    float Yaw;
    float Pitch;
    // float Roll;

    float MovementSpeed;
    float MouseSensitivity;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

    glm::mat4 GetRotateMatrix();

    void ProcessKeyboard(CameraMovement direction, float deltaTime);

    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

private:
    void UpdateCameraVectors();
};

Camera::Camera(glm::vec3 position)
             : Position(position),
               Front(Global::WorldFront),
               //Up(Global::WorldUp),
               Right(Global::WorldRight),
               Yaw(0.0f), Pitch(0.0f), //Roll(0.0f),
               MovementSpeed(Global::CameraSpeed),
               MouseSensitivity(Global::CameraSensitivity)
{

}

glm::mat4 Camera::GetRotateMatrix()
{
    glm::mat4 rotate = glm::identity<glm::mat4>();
    rotate = glm::rotate(rotate, glm::radians(-Pitch), Right);
    rotate = glm::rotate(rotate, glm::radians(Yaw), Global::WorldUp);
    // rotate = glm::rotate(rotate, glm::radians(Roll), Front);

    return rotate;
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
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
    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;

    front.x = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
    front.y = sin(glm::radians(Pitch));
    front.z = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));

    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Global::WorldUp, Front)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
}

#endif