#version 130


//utils.glsl is included manually, done in CPP code
__UTILS.GLSL__


uniform vec2 worldSize;

uniform float seed1; //used for computing initial position randomly
uniform float seed2;


/* Initializes positions randomly in the world frame */
void main()
{
    /* Random vector in [0,1]x[0,1] */
    vec2 randomVector = vec2(random(gl_FragCoord.xy * seed1),
                             random(gl_FragCoord.yx * seed2));
    
    vec2 pos = (-worldSize * 0.5) + (randomVector * worldSize);
    
    gl_FragColor = coordsToColor(pos, MAX_POSITION);
}
