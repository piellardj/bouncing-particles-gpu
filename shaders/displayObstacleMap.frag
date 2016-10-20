#version 130

uniform sampler2D obstacleMap;
uniform vec2 bufferSize;


__UTILS.GLSL__



void main()
{
    vec2 coords = gl_FragCoord.xy / bufferSize;
    vec2 normal = colorToCoords(texture(obstacleMap, coords),
                                MAX_NORMAL);
    
    const vec4 color = vec4(1);
    gl_FragColor = color * step(0.001, dot(normal,normal));
}
