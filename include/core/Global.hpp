#ifndef GLOBAL_HPP
#define GLOBAL_HPP

namespace Global
{
    // path tracing arguments----------------------------------------------------------------------

    const int spp = 128;
    const float RussianRoulette = 0.8f;
    const float IndirLightContributionRate = 1;

    // constants-----------------------------------------------------------------------------------

    const float Pi = 3.1415926535897f;

    // inline functions----------------------------------------------------------------------------

    inline float deg2rad(const float &deg) { return deg * Pi / 180.0; }

    inline float clamp(const float &lo, const float &hi, const float &v) { return std::max(lo, std::min(hi, v)); }

    // image configuration-------------------------------------------------------------------------

    enum ImageType { PNG, JPG, PPM };
    const std::string EnumString[] = { "png", "jpg", "ppm"};
    const std::string ImagePath = ".\\image\\";
    const std::string Author = "# Author: zionFisher GitHub: https://github.com/zionFisher\n# 2021";
    const ImageType ImageFileType = PNG;
    const std::string ImageName = ImagePath + "result_spp_" + std::to_string(spp) + "." + EnumString[ImageFileType];

    // camera configuration------------------------------------------------------------------------

    const float OriginX = 278;
    const float OriginY = 273;
    const float OriginZ = -800;
    const glm::vec3 CameraPos = glm::vec3(OriginX, OriginY, OriginZ);

    const glm::vec3 WorldFront = glm::vec3(0.0f, 0.0f, 1.0f);
    const glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 WorldLeft = glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 WorldRight = -WorldLeft;

    const float CameraYaw = 0.0f;
    const float CameraPitch = 0.0f;
    const float CameraRoll = 0.0f;
    const float CameraSpeed = 100.0f;
    const float CameraSensitivity = 0.1f;

    // window configuration------------------------------------------------------------------------

    const std::string WindowName = "Super Simple Path Tracing Renderer.";
    const unsigned int WindowWidth = 784;
    const unsigned int WindowHeight = 784;
    const unsigned int PixelCount = WindowWidth * WindowHeight;

    const float ImageAspectRatio = WindowWidth / (float)WindowHeight;
    const unsigned int FOV = 40;
    const float Scale = std::tan(deg2rad(FOV * 0.5));

}

#endif