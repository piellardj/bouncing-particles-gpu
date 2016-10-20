#version 130


uniform sampler2D positions;
uniform mat3 viewMatrix;

attribute vec2 coordsOnBuffer;
attribute vec4 color;

out vec4 fragColor;


__UTILS.GLSL__


void main()
{
    vec2 pos2D = colorToCoords(texture2D(positions, coordsOnBuffer), MAX_POSITION);

    gl_Position = vec4(viewMatrix * vec3(pos2D, 1.0), 1.0);

    fragColor = color;
}
