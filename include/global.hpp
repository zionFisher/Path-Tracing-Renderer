#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <cmath>
#include <iostream>

float deg2rad(const float &deg);

const float Pi = 3.1415926535897f;

const unsigned int ScreenWidth = 784;
const unsigned int ScreenHeight = 784;
const unsigned int PixelCount = ScreenWidth * ScreenHeight;

const float imageAspectRatio = ScreenWidth / (float)ScreenHeight;
const unsigned int FOV = 40;
const float scale = std::tan(deg2rad(FOV * 0.5));

const int spp = 2;

const float RussianRoulette = 0.8f;
const float IndirLightContributionRate = 1;

float deg2rad(const float &deg)
{
    return deg * Pi / 180.0;
}

inline float clamp(const float &lo, const float &hi, const float &v)
{
    return std::max(lo, std::min(hi, v));
}

#endif