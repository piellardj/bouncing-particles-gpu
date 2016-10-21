#version 130

uniform sampler2D obstacleMap;
uniform vec2 bufferSize;
uniform vec4 color = vec4(.6, .6, .6, 1.0);


//utils.glsl is included manually, done in CPP code
__UTILS.GLSL__



void main()
{
    vec2 coords = gl_FragCoord.xy / bufferSize;
    vec2 normal = colorToCoords(texture(obstacleMap, coords),
                                MAX_NORMAL);
    
    gl_FragColor = color * step(0.001, dot(normal,normal));
}
