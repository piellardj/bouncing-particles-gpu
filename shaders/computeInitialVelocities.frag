#version 130


__UTILS.GLSL__


void main()
{
    gl_FragColor = coordsToColor(vec2(0,0), MAX_SPEED);
}
