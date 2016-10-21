#version 130


//utils.glsl is included manually, done in CPP code
__UTILS.GLSL__


/* Initializes the obstacle map to be empty */
void main()
{
    gl_FragColor = coordsToColor(vec2(0.0), MAX_NORMAL);
}
