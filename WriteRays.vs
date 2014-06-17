#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D OppositePositions;

     in  vec3  Position;
     in  vec2  TexCoord;
     out vec2  FragTexCoord;
flat out float Value;

void main()
{
    ivec2 pos0    = ivec2(Position.xy);
    // fetch not available on Mali GPU's T_T
    float pos1_id = fetch(OppositePositions, pos0).r;
    ivec2 pos1    = decode(pos1_id);
    // todo: replace GL_MAX with GL_MIN
    Value         = 50 - distance(pos0, pos1);
    
    vec2 p1 = getScreenSpaceCoord(OppositePositions, pos0);
    vec2 p2 = getScreenSpaceCoord(OppositePositions, pos1);

    // todo: remove
    FragTexCoord = TexCoord;
    // Position.z is 1 if the position needs to be scattered to pos1, 0 otherwise ("scatter" to pos0).
    // mix does x * a + y * (1-a)
    gl_Position = vec4( mix(p2, p1, Position.z), 0, 1);
}