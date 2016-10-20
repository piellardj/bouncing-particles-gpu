#version 130


uniform sampler2D positions;
uniform sampler2D oldVelocities;

uniform sampler2D obstacleMap;


uniform vec2 worldSize;
uniform vec2 bufferSize;

uniform vec2 acceleration;

uniform float dt;


__UTILS.GLSL__


vec2 getObstacleNormal (const vec2 position)
{
    vec2 coordsOnObstacleMap = position / worldSize + vec2(0.5, 0.5);
    
    return colorToCoords(texture(obstacleMap, coordsOnObstacleMap),
                         MAX_NORMAL);
}

vec2 getAcceleration(const vec2 coordsOnBuffer)
{
    return acceleration;
}

vec2 getNewVelocity(const vec2 coordsOnBuffer)
{
    vec2 position = colorToCoords(texture(positions, coordsOnBuffer),
                                  MAX_POSITION);
    
    vec2 velocity = colorToCoords(texture(oldVelocities, coordsOnBuffer),
                                  MAX_SPEED);
    vec2 acceleration = getAcceleration(coordsOnBuffer);
    
    //add current acceleration
    vec2 newVelocity = velocity + dt * acceleration;
    
    if (position.y < -worldSize.y/2.0) {
        newVelocity = vec2(0,0);
    }
    
    vec2 obstacleNormal = getObstacleNormal(position);
    if (length(obstacleNormal) > 0.01) {
        newVelocity = reflect(newVelocity, obstacleNormal) + dt * obstacleNormal;
        newVelocity = newVelocity * 0.5;
    }
    
    return newVelocity;
}

void main()
{
    vec2 coordsOnBuffer = gl_FragCoord.xy / bufferSize;

    gl_FragColor = coordsToColor(getNewVelocity(coordsOnBuffer),
                                 MAX_SPEED);
}
