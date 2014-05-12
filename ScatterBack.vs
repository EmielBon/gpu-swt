#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;
uniform int       Column;

     in  vec3  Position;
     in  vec2  TexCoord;
     out vec2  FragTexCoord;
flat out float ScatterID;

void main()
{
    ivec2 current_xy       = ivec2(Position.xy) + ivec2(Column, 0);
    float current_root_id  = fetch(Texture, current_xy).z;
    ivec2 neighbor_xy      = current_xy + ivec2(1, 0);
    float neighbor_root_id = fetch(Texture, neighbor_xy).z;
    ivec2 neighbor_root_xy = decode(neighbor_root_id);
    ScatterID = current_root_id;
    FragTexCoord = TexCoord;
    gl_Position = vec4(getScreenSpaceCoord(Texture, neighbor_root_xy), (neighbor_root_xy.x + neighbor_root_xy.y * 800.0) / (800 * 600), 1);
}