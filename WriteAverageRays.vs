#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D OppositePositions;
uniform sampler2D AverageValues;

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
    Value         = fetch(AverageValues, pos0).r;
    
    vec2 p1 = getScreenSpaceCoord(OppositePositions, pos0);
    vec2 p2 = getScreenSpaceCoord(OppositePositions, pos1);
    
    FragTexCoord = TexCoord;
    gl_Position = vec4( mix(p2, p1, Position.z), 0, 1);
}