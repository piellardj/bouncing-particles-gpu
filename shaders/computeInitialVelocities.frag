#version 130


//utils.glsl is included manually, done in CPP code
__UTILS.GLSL__


uniform float maxInitialSpeed = 1.0;
uniform float seed1;
uniform float seed2;


/* Initializes velocities randomly within a [0,maxInitialSpeed] speed range. */
void main()
{
    vec2 speed = vec2(random(gl_FragCoord.xy * seed1),
                      random(gl_FragCoord.yx * seed2));
    speed = (2*speed - 1) * maxInitialSpeed;
    
    gl_FragColor = coordsToColor(speed, MAX_SPEED);
}
