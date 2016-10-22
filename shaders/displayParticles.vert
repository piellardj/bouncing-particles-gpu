#version 130


uniform sampler2D positions;
uniform sampler2D velocities;

uniform mat3 viewMatrix;

uniform vec4 colorSlow = vec4(1.0, 0.9, 0.9, 1.0);
uniform vec4 colorFast = vec4(1.0, 0.0, 0.0, 1.0);

attribute vec2 coordsOnBuffer;

out vec4 fragColor;


__UTILS.GLSL__


/* Displays particles with color related to speed */
void main()
{
    vec2 pos2D = colorToCoords(texture2D(positions, coordsOnBuffer), MAX_POSITION);
    vec2 velocity = colorToCoords(texture2D(velocities, coordsOnBuffer), MAX_SPEED);
    
    gl_Position = vec4(viewMatrix * vec3(pos2D, 1.0), 1.0);
    
    float speed = length(velocity);
    float r = speed / (MAX_SPEED / 2.0);
    r = clamp (r, 0.0, 1.0);
    
    fragColor = colorSlow * (1 - r) + colorFast * r;
}
