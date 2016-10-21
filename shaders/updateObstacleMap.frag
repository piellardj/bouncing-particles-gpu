#version 130

uniform sampler2D oldObstacleMap;
uniform vec2 bufferSize;

uniform vec2 newObstacleCenter;
uniform float isFullObstacle; //false (<0) meanss remove obstacle
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
    
    if (distance < radius) {
        if (isFullObstacle < 0) {
            gl_FragColor = coordsToColor(vec2(0,0), MAX_NORMAL);
        } else {
            vec2 newNormal = (fromCenter / distance) * step(distance, radius);
            
            /* we only normalize if the vector is /= (0,0) to avoid undefined behaviour */
            if (dot(newNormal,newNormal) > 0.001) {
                newNormal = normalize(newNormal);
            }
            
            newNormal = normalize(oldNormal + newNormal);
            
            gl_FragColor = coordsToColor(newNormal, MAX_NORMAL);
        }
    } else {
        gl_FragColor = oldColor;
    }
}
