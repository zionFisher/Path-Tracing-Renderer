#ifndef FRAMESAVER_HPP
#define FRAMESAVER_HPP

#include "Global.hpp"
#include <fstream>

struct RGBColor
{
    unsigned int R;
    unsigned int G;
    unsigned int B;
};

using ResultColor = RGBColor;

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

    int counter = 0;

    void WriteAuthor(std::ofstream &outStream);

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
    colorBuffer = new RGBColor[Global::PixelCount];
}

FrameSaver::~FrameSaver()
{
    delete[] colorBuffer;
}

void FrameSaver::SaveBuffer()
{
    glReadPixels(0, 0, Global::ScreenWidth, Global::ScreenHeight, GL_RGB, GL_UNSIGNED_INT, colorBuffer);
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

void FrameSaver::WriteAuthor(std::ofstream &outStream)
{
    if (!outStream.is_open())
        return;

    outStream << Global::Author << std::endl;
}

void FrameSaver::WritePPM(const char *fileName)
{
    std::ofstream outStream;
    outStream.open(fileName);
    if (!outStream.is_open())
        return;

    outStream << "P3\n"
              << Global::ScreenWidth << " " << Global::ScreenHeight << std::endl
              << "255\n";

    for (int i = Global::PixelCount - 1; i > -1; --i)
    {
        outStream << colorBuffer[i].R << " "
                  << colorBuffer[i].G << " "
                  << colorBuffer[i].B << std::endl;
        // outStream << resultBuffer[i].R << " "
        //           << resultBuffer[i].G << " "
        //           << resultBuffer[i].B << std::endl;
    }

    WriteAuthor(outStream);

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