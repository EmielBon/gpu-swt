#version 150

#pragma include TextureUtil.fsh

uniform sampler2D Texture;

in vec3 Position;
in vec2 TexCoord;

void main()
{
    float l = texture(Texture, TexCoord).r;
    
}