#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;

      in  vec3  Position;
float out float RootID;

void main()
{
    ivec2 current_xy       = ivec2(Position.xy);
    float current_root_id  = fetch(Texture, current_xy).z;
    ivec2 neighbor_xy      = current_xy + ivec2(1, 0);
    float neighbor_root_id = fetch(Texture, neighbor_xy).z;
    ivec2 neighbor_root_xy = decode(neighbor_root_id);
    RootID = current_root_id;
    gl_Position = vec4(getScreenSpaceCoord(Texture, neighbor_root_xy), 1, 1);
}