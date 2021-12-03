#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <cmath>

namespace Global
{
    float deg2rad(const float &deg);

    float clamp(const float &lo, const float &hi, const float &v);

    const std::string Author = "# Author: zionFisher GitHub: https://github.com/zionFisher\n# 2021";

    const char *WindowName = "Super Simple Path Tracing Renderer.";

    const float Pi = 3.1415926535897f;

    const float OriginX = 278;
    const float OriginY = 273;
    const float OriginZ = -800;

    const glm::vec3 WorldFront = glm::vec3(0.0f, 0.0f, 1.0f);
    const glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 WorldRight = glm::vec3(1.0f, 0.0f, 0.0f);
    const float CameraYaw = 0.0f;
    const float CameraPitch = 0.0f;
    const float CameraRoll = 0.0f;
    const float CameraSpeed = 100.0f;
    const float CameraSensitivity = 0.1f;

    const unsigned int ScreenWidth = 784;
    const unsigned int ScreenHeight = 784;
    const unsigned int PixelCount = ScreenWidth * ScreenHeight;

    const float ImageAspectRatio = ScreenWidth / (float)ScreenHeight;
    const unsigned int FOV = 40;
    const float Scale = std::tan(deg2rad(FOV * 0.5));

    const int spp = 1;

    const float RussianRoulette = 0.8f;
    const float IndirLightContributionRate = 1;

    inline float deg2rad(const float &deg) { return deg * Pi / 180.0; }

    inline float clamp(const float &lo, const float &hi, const float &v) { return std::max(lo, std::min(hi, v)); }
}

#endif