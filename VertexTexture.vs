#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;

in  vec3 Position;
in  vec2 TexCoord;
out vec2 FragTexCoord;

flat out vec4 Color;

void main()
{
    FragTexCoord = TexCoord;
    vec2 pos     = getScreenSpaceCoord(Texture, Position.xy);
    Color        = fetch(Texture, Position.xy);
    gl_Position  = vec4(pos, 0, 1);
}