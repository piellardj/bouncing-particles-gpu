#version 130


uniform vec2 worldSize;
uniform vec2 bufferSize;


__UTILS.GLSL__


void main()
{
    float nbParticles = bufferSize.x * bufferSize.y;
    float index = gl_FragCoord.x + gl_FragCoord.y*bufferSize.x;
    
    float worldLeft = -worldSize.x / 2.0;
    float worldRight = worldSize.x / 2.0;
    
    vec2 pos = vec2(worldLeft + index/nbParticles * worldSize.x, worldSize.y/2);
    
    gl_FragColor = coordsToColor(pos, MAX_POSITION);
}
