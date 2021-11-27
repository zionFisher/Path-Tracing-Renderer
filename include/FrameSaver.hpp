#ifndef FRAMESAVER_HPP
#define FRAMESAVER_HPP

#include "global.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <fstream>

struct RGBColor
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
};

enum ImageType
{
    PPM,
    BMP,
    PNG,
    JPG
};

class FrameSaver
{
private:
    bool bufferIsSaved;

    RGBColor *colorBuffer; // color buffer

    void WritePPM(const char *fileName);
    void WriteBMP(const char *fileName);
    void WritePNG(const char *fileName);
    void WriteJPG(const char *fileName);

public:
    FrameSaver();
    ~FrameSaver();

    void SaveBuffer();
    void SaveImage(const char *fileName, ImageType type);
};

FrameSaver::FrameSaver() : bufferIsSaved(false)
{
    colorBuffer = new RGBColor[PixelCount];
}

FrameSaver::~FrameSaver()
{
    delete[] colorBuffer;
}

void FrameSaver::SaveBuffer()
{
    glReadPixels(0, 0, ScreenWidth, ScreenHeight, GL_RGB, GL_UNSIGNED_BYTE, colorBuffer);
    bufferIsSaved = true;
}

void FrameSaver::SaveImage(const char *fileName, ImageType type)
{
    if (!bufferIsSaved)
        return;

    switch (type)
    {
    case PPM:
        WritePPM(fileName);
        break;
    case BMP:
        WriteBMP(fileName);
        break;
    case PNG:
        WritePNG(fileName);
        break;
    case JPG:
        WriteJPG(fileName);
        break;

    default:
        WritePPM(fileName);
        break;
    }
}

void FrameSaver::WritePPM(const char *fileName)
{
    std::ofstream outStream;
    outStream.open(fileName);
    if (!outStream.is_open())
        return;

    outStream << "P3\n"
              << ScreenWidth << " " << ScreenHeight << std::endl
              << "255\n";

    for (int  i = PixelCount - 1; i > -1; --i)
    {
        // outStream << "255 " << "255 " << "255\n"; // for test
        // outStream << (unsigned int)(255 * std::pow(clamp(0, 1, colorBuffer[i].R), 0.6f)) << " "
        //           << (unsigned int)(255 * std::pow(clamp(0, 1, colorBuffer[i].G), 0.6f)) << " "
        //           << (unsigned int)(255 * std::pow(clamp(0, 1, colorBuffer[i].B), 0.6f)) << std::endl;
        outStream << (unsigned int)(colorBuffer[i].R) << " "
                  << (unsigned int)(colorBuffer[i].G) << " "
                  << (unsigned int)(colorBuffer[i].B) << std::endl;
    }

    outStream.close();
}

void FrameSaver::WriteBMP(const char *fileName)
{
    throw -1;
}

void FrameSaver::WritePNG(const char *fileName)
{
    throw -1;
}

void FrameSaver::WriteJPG(const char *fileName)
{
    throw -1;
}

#endif