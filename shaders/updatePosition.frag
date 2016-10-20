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
        float index = gl_FragCoord.x + gl_FragCoord.y*bufferSize.x;
        
        float worldLeft = -worldSize.x / 2.0;
        float worldTop = worldSize.y / 2.0;
    
        newPosition = vec2(worldLeft + 2.0 * mod(index, worldSize.x),
                           worldTop + 3.0 * floor(index / worldSize.x));
    }
    
    gl_FragColor = coordsToColor(newPosition, MAX_POSITION);
}
