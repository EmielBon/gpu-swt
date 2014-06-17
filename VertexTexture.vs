#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;

in  vec3 Position;
in  vec2 TexCoord;

flat out vec4 Color;

void main()
{
    vec2 pos     = getScreenSpaceCoord(Texture, Position.xy);
    Color        = fetch(Texture, Position.xy);
    gl_Position  = vec4(pos, 0, 1);
}