#ifndef FRAMESAVER_HPP
#define FRAMESAVER_HPP

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Global.hpp"
#include <fstream>
#include <cstring>
#include <stbi/stb_image_write.hpp>

class FrameSaver
{
private:
    bool bufferIsSaved;

    unsigned char *colorBuffer;
    float *multiSampleBuffer;

    unsigned int counter;

    void WriteAuthor(std::ofstream &outStream);

    void WritePNG(const char *fileName);
    void WriteJPG(const char *fileName);
    void WritePPM(const char *fileName);

public:
    FrameSaver();
    ~FrameSaver();

    void SaveBuffer();
    void SaveImage(const char *fileName, Global::ImageType type);
};

FrameSaver::FrameSaver() : bufferIsSaved(false), counter(0)
{
    colorBuffer = new unsigned char[3 * Global::PixelCount];
    multiSampleBuffer = new float[3 * Global::PixelCount];
}

FrameSaver::~FrameSaver()
{
    delete[] colorBuffer;
    delete[] multiSampleBuffer;
}

void FrameSaver::SaveBuffer()
{
    if (counter > Global::spp)
        return;

    if (counter == Global::spp)
    {
        for (int i = 0; i < 3 * Global::PixelCount; i++)
            colorBuffer[i] = (unsigned char)multiSampleBuffer[i];
        return;
    }

    counter++;
    glReadBuffer(GL_FRONT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glReadPixels(0, 0, Global::WindowWidth, Global::WindowHeight, GL_RGB, GL_UNSIGNED_BYTE, colorBuffer);

    for (int i = 0; i < 3 * Global::PixelCount; i++)
        multiSampleBuffer[i] += ((float)colorBuffer[i] / (float)Global::spp);

    bufferIsSaved = true;
}

void FrameSaver::SaveImage(const char *fileName, Global::ImageType type)
{
    if (!bufferIsSaved)
        return;

    switch (type)
    {
    case Global::ImageType::PNG:
        WritePNG(fileName);
        break;
    case Global::ImageType::JPG:
        WriteJPG(fileName);
        break;
    case Global::ImageType::PPM:
        WritePPM(fileName);
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

void FrameSaver::WritePNG(const char *fileName)
{
    stbi_flip_vertically_on_write(true);
    stbi_write_png(fileName, Global::WindowWidth, Global::WindowHeight, 3, colorBuffer, Global::WindowWidth * 3);
}

void FrameSaver::WriteJPG(const char *fileName)
{
    stbi_flip_vertically_on_write(true);
    stbi_write_jpg(fileName, Global::WindowWidth, Global::WindowHeight, 3, colorBuffer, 100);
}

/* PPM output image format:
 * P3
 * Global::WindowWidth Global::WindowHeight
 * 255
 * 0 0 0
 * 0 0 0
 * 255 255 255
 * ...
 * # Author: ...
 */
void FrameSaver::WritePPM(const char *fileName)
{
    std::ofstream outStream;
    outStream.open(fileName);
    if (!outStream.is_open())
        return;

    outStream << "P3" << std::endl
              << Global::WindowWidth << " " << Global::WindowHeight << std::endl
              << "255" << std::endl;

    // Framebuffer starts from the lower left to the upper right
    // PPM Image starts from the upper left to the lower right
    for (int row = Global::WindowHeight - 1; row > -1; row--)
    {
        for (int column = 0; column < Global::WindowWidth; column++)
        {
            int preIndex = row * Global::WindowWidth * 3;
            int curIndex = preIndex + column * 3;
            outStream << (unsigned int)colorBuffer[curIndex] << " "
                      << (unsigned int)colorBuffer[curIndex + 1] << " "
                      << (unsigned int)colorBuffer[curIndex + 2];

            if (column == Global::WindowWidth - 1)
                outStream << std::endl;
            else
                outStream << " ";
        }
    }

    WriteAuthor(outStream);

    outStream.close();
}

#endif