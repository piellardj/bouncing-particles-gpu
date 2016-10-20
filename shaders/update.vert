#version 130

attribute vec2 position;

out vec2 persoPos;

void main()
{
    persoPos = position;
    gl_Position = vec4(position, 0.0, 1.0);
}
