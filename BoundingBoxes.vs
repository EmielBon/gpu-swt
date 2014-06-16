#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;

in  vec3  Position;
in  vec2  TexCoord;
out vec2  FragTexCoord;

flat out float ScatterID;
flat out vec2  CurrentRootXY;

void main()
{
    ivec2 current_xy      = ivec2(Position.xy);
    float current_root_id = fetch(Texture, current_xy).a;
    vec2  current_root_xy = decode(current_root_id);
    
    CurrentRootXY = current_xy;
    // todo: remove
    FragTexCoord  = TexCoord;
    
    vec2 pos = getScreenSpaceCoord(Texture, current_root_xy);
    gl_Position = vec4(pos, 0, 1);
}