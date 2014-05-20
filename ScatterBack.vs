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
    // todo: fetch not available on Mali GPUs' T_T 
    float current_root_id  = fetch(Texture, current_xy).a;
    ivec2 current_root_xy  = decode(current_root_id);
    ivec2 neighbor_xy      = current_xy + ivec2(1, 0);
    float neighbor_root_id = fetch(Texture, neighbor_xy).a;
    ivec2 neighbor_root_xy = decode(neighbor_root_id);
    ScatterID = current_root_id;
    FragTexCoord = TexCoord;
    vec2 dims = vec2( size(Texture) );
    gl_Position = vec4(getScreenSpaceCoord(Texture, neighbor_root_xy), /*current_root_id / 10*/(current_root_xy.x + current_root_xy.y * dims.x) / (dims.x * dims.y), 1);
}