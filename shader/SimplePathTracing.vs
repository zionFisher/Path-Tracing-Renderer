#version 330 core
#define LEFT_HAND_COORDS
layout (location = 0) in vec3 rDir;
layout (location = 1) in vec2 wCoord;

out vec3 rayDirection;
out vec3 eye;
out vec2 screen;

uniform vec2 Screen;
uniform vec3 Eye;

void main()
{
    rayDirection = rDir;
    eye = Eye;
	screen = Screen;

#ifdef LEFT_HAND_COORDS
    gl_Position = vec4(-wCoord.x, wCoord.y, 0.0f, 1.0); // Left-Hand Coordinate
#else
    gl_Position = vec4(wCoord.x, wCoord.y, 0.0f, 1.0);
#endif
}