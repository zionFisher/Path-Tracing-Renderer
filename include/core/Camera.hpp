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
    glm::vec3 Left;
    glm::vec3 Up;

    float Yaw;
    float Pitch;
    // float Roll;

    float MovementSpeed;
    float MouseSensitivity;

    float *vertices;

    Camera(glm::vec3 position = Global::CameraPos,
           glm::vec3 front = Global::WorldFront,
           glm::vec3 left = Global::WorldLeft);

    ~Camera();

    void GenerateRay();

    glm::mat4 GetRotateMatrix();

    void ProcessKeyboard(CameraMovement direction, float deltaTime);

    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

private:
    void UpdateCameraVectors();
};

Camera::Camera(glm::vec3 position,
               glm::vec3 front,
               glm::vec3 left)
    : Position(position),
      Front(front),
      Left(left),
      Yaw(0.0f),
      Pitch(0.0f),
      //Roll(0.0f),
      MovementSpeed(Global::CameraSpeed),
      MouseSensitivity(Global::CameraSensitivity)
{
    vertices = new float[Global::PixelCount * 5];
}

Camera::~Camera()
{
    delete[] vertices;
}

void Camera::GenerateRay()
{
    int counter = 0;

    for (int j = 0; j < Global::ScreenHeight; ++j)
    {
        for (int i = 0; i < Global::ScreenWidth; ++i)
        {
            float worldSpaceCoordX = 2 * ((float)i + 0.5) / (float)Global::ScreenWidth - 1;
            float worldSpaceCoordY = 2 * ((float)j + 0.5) / (float)Global::ScreenHeight - 1; // OpenGL 屏幕坐标原点在左下角

            float x = worldSpaceCoordX * Global::ImageAspectRatio * Global::Scale;
            float y = worldSpaceCoordY * Global::Scale;
            glm::vec3 dir = normalize(glm::vec3(x, y, 1));

            for (int temp = 0; temp < 3; temp++)
                vertices[counter++] = dir[temp];
            vertices[counter++] = worldSpaceCoordX;
            vertices[counter++] = worldSpaceCoordY;
        }
    }
}

glm::mat4 Camera::GetRotateMatrix()
{
    glm::mat4 rotate = glm::identity<glm::mat4>();
    rotate = glm::rotate(rotate, glm::radians(-Pitch), Left);
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
        Position += Left * velocity;
    if (direction == RIGHT)
        Position -= Left * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw -= xoffset;
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
    Left = glm::normalize(glm::cross(Global::WorldUp, Front)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Front, Left));
}

#endif