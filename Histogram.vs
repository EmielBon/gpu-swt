#version 150

#pragma include TextureUtil.fsh

uniform sampler2D Texture;

in  vec3 Position;
in  vec2 TexCoord;
out vec2 FragTexCoord;

void main()
{
    float l = texture(Texture, TexCoord).r;
    
}