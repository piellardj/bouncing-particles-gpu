#version 130

uniform sampler2D oldObstacleMap;
uniform vec2 bufferSize;

uniform vec2 newObstacleCenter;
uniform float hardness;
uniform float radius;


__UTILS.GLSL__


void main()
{
    vec4 oldColor = texture(oldObstacleMap, gl_FragCoord.xy / bufferSize);
    vec2 oldNormal = colorToCoords(oldColor,
                                   MAX_NORMAL);
   
    vec2 fromCenter = gl_FragCoord.xy - newObstacleCenter;
    float distance = length(fromCenter);
    vec2 normal = (fromCenter / distance) * step(distance, radius);
    
    vec2 newNormal = (normal + oldNormal) * hardness;
    
    /* we only normalize if the vector is /= (0,0) to avoid undefined behaviour */
    if (dot(newNormal,newNormal) > 0.001) {
        newNormal = normalize(normal + oldNormal);
    }
    
    gl_FragColor = coordsToColor(newNormal, MAX_NORMAL);
}
