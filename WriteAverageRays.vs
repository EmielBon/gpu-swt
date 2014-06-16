#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

//uniform sampler2D Gradients;
uniform sampler2D OppositePositions;
uniform sampler2D AverageValues;
//uniform bool      DarkOnLight;

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
    //vec2 gradient    = fetch(Gradients, pos).xy;
    //gradient         = normalize(gradient) * (DarkOnLight ? 1 : -1);
    Value         = fetch(AverageValues, pos0).r;
    
    // todo: replace z with GL_MAX
    float z = Value / 50;
    vec4 p1 = vec4(getScreenSpaceCoord(OppositePositions, pos0), z, 1);
    vec4 p2 = vec4(getScreenSpaceCoord(OppositePositions, pos1), z, 1);
    
    if (Position.z == 0.0)
        gl_Position = p1;
    else gl_Position = p2;
    
    FragTexCoord = TexCoord;
}