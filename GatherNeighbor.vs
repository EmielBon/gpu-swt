#version 150

#pragma include TextureUtil.fsh
#pragma include Codec.fsh

uniform sampler2D Texture;
uniform int       Column;

     in  vec3  Position;
     in  vec2  TexCoord;
     out vec2  FragTexCoord;
flat out float ScatterID;

void main()
{
    ivec2 current_xy      = ivec2(Position.xy) + ivec2(Column, 0);
    float current_root_id = fetch(Texture, current_xy).a;
    ivec2 current_root_xy = decode(current_root_id);
    
    ivec2 neighbor_xy      = current_xy + ivec2(1, 0);
    float neighbor_root_id = fetch(Texture, neighbor_xy).a;
    
    FragTexCoord = TexCoord;
    ScatterID    = neighbor_root_id;
    
    gl_Position = vec4(getScreenSpaceCoord(Texture, current_root_xy), 0, 1);
}