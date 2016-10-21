/* These functions help storing float values into a texture's color channels.
   One float takes two color channels -> 256*256 = 65526 possible values.

   To take full advantage of the two channels, we make assumtpions on
   the range of the storable float values. The smaller the range is, the
   more precise the storage will be.
    - storable speed is between [-0.5;0.5] * MAX_SPEED
    - storable position is between [-0.5;0.5] * MAX_POSITION

    If a value exceeds the expected range, we clamp it.

    To store a float value, we scale it to fit in [0, 65535],
    then project it on the base 256.
*/


const float MAX_SPEED = 16.0;
const float MAX_POSITION = 2048.0;
const float MAX_NORMAL = 2.0;


/*  Used as simple rand() function, co is the seed.*/
float random(vec2 co)
{
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

/* Converts value stored in two color channels
   to float value in [0, 65535] */
float fromBase256 (const vec2 digits)
{
    return dot(vec2(255.0*256.0, 255.0), digits);
}

/* Converts float value in [0, 65535]
   to value storable in two color channels */
vec2 toBase256 (float value)
{
    value = clamp (value, 0.0, 65535.0);
    
    float strongComponent = floor(value / 256.0);
    float weakComponent = value - 256.0 * strongComponent;
    
    return vec2(strongComponent, weakComponent) / 255.0;
}

/* coords' components supposed to be in [-zoneWidth/2, zoneWidth/2] */
vec4 coordsToColor(const vec2 coords, const float zoneWidth)
{
    /* First we map the original value from [-zoneWidth/2, zoneWidth/2]
       to [0, 65535], then encode it */
    vec2 scaledCoords = (coords/zoneWidth + vec2(0.5)) * 65535.0;
    
    return vec4(toBase256(scaledCoords.x), toBase256(scaledCoords.y));
}

/* color's components are supposed to be in [0,1] */
vec2 colorToCoords(const vec4 color, const float zoneWidth)
{
    /* First we read the value in [0, 65535] and then map it
       to [-zoneWidth/2, zoneWidth/2] */
    vec2 scaledCoords = vec2(fromBase256(color.rg), fromBase256(color.ba));
    
    return (scaledCoords / 65535.0 - vec2(0.5)) * zoneWidth;
}
