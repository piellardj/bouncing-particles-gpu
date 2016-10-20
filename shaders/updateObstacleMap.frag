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
    
    if (distance < radius) {
        vec2 normal = (fromCenter / distance) * step(distance, radius);
        
        /* we only normalize if the vector is /= (0,0) to avoid undefined behaviour */
        if (dot(normal,normal) > 0.001) {
            normal = normalize(normal);
        }
        gl_FragColor = coordsToColor(normal * hardness, MAX_NORMAL);
    } else {
        gl_FragColor = oldColor;
    }
}
