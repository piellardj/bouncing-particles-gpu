#version 130


uniform sampler2D oldPositions;
uniform sampler2D velocities;

uniform sampler2D obstacleMap;

uniform vec2 worldSize;
uniform vec2 bufferSize;

uniform float dt;


//utils.glsl is included manually, done in CPP code
__UTILS.GLSL__


vec2 getObstacleNormal (const vec2 position)
{
    vec2 coordsOnObstacleMap = position / worldSize + vec2(0.5, 0.5);
    
    return colorToCoords(texture(obstacleMap, coordsOnObstacleMap),
                         MAX_NORMAL);
}

void main()
{
    vec2 coordsOnBuffer = gl_FragCoord.xy / bufferSize;
    
    /* Retrieving of position and velocity from texture buffers */
    vec2 position = colorToCoords(texture(oldPositions, coordsOnBuffer),
                                  MAX_POSITION);
    vec2 velocity = colorToCoords(texture(velocities, coordsOnBuffer),
                                  MAX_SPEED);
    
    vec2 newPosition = position + dt*velocity;
    
    /* If the particle is in an obstacle, we move it to the edge */
    newPosition = newPosition + dt*getObstacleNormal(newPosition);
    
    /* Particles too low are placed randomly on top again */
    if (position.y < -worldSize.y/2.0) {
        float worldLeft = -worldSize.x / 2.0;
        float worldTop = worldSize.y / 2.0;
        
        newPosition = vec2(worldLeft + random(newPosition) * worldSize.x,
                           worldTop - random(position) * 5);
    }
    
    gl_FragColor = coordsToColor(newPosition, MAX_POSITION);
}
