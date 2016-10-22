#version 130

uniform sampler2D oldObstacleMap;
uniform vec2 bufferSize;

uniform vec2 newObstacleCenter;
uniform float radius;


//utils.glsl is included manually, done in CPP code
__UTILS.GLSL__


/* Adds a circular obstacle, blending it with any existing obstacle on that spot */
void main()
{
    vec4 oldColor = texture(oldObstacleMap, gl_FragCoord.xy / bufferSize);
    vec2 oldNormal = colorToCoords(oldColor,
                                   MAX_NORMAL);
   
    vec2 fromCenter = gl_FragCoord.xy - newObstacleCenter;
    float distance = length(fromCenter);
    
    vec2 additionalNormal = normalize(fromCenter) * step(distance, radius);
    
    vec2 newNormal = oldNormal + additionalNormal;
    if (dot(newNormal, newNormal) > 0.0001)
        newNormal = normalize(newNormal);
    
    gl_FragColor = coordsToColor(newNormal, MAX_NORMAL);
}
