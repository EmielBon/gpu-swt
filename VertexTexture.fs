#pragma include Codec.fsh
#pragma include TextureUtil.fsh

//uniform sampler2D Texture;

out vec4 FragColor;

uniform vec4 Color;

void main()
{
    FragColor = Color;
}