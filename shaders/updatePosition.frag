#version 130


uniform sampler2D oldPositions;
uniform sampler2D velocities;

uniform vec2 worldSize;
uniform vec2 bufferSize;

uniform float dt;


__UTILS.GLSL__


void main()
{
    vec2 coordsOnBuffer = gl_FragCoord.xy / bufferSize;
    
    /* Retrieving of position and velocity from texture buffers */
    vec2 position = colorToCoords(texture(oldPositions, coordsOnBuffer),
                                  MAX_POSITION);
    vec2 velocity = colorToCoords(texture(velocities, coordsOnBuffer),
                                  MAX_SPEED);

    vec2 newPosition = position + dt*velocity;
    
    if (position.y < -worldSize.y/2.0) {
        newPosition.y = worldSize.y/2.0;
    }
    
    gl_FragColor = coordsToColor(newPosition, MAX_POSITION);
}
