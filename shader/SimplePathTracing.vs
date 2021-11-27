#version 330 core
layout (location = 0) in vec3 rDir;
layout (location = 1) in vec2 wCoord;

out vec3 rayDirection;
out vec3 origin;
out vec2 screen;

uniform vec2 Screen;
uniform vec3 Eye;

void main()
{
    rayDirection = rDir;
    origin = Eye;
	screen = Screen;

    gl_Position = vec4(wCoord.x, wCoord.y, 0.0f, 1.0);
}