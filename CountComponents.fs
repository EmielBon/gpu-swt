#version 150

#pragma include Util.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;

flat in  ivec2 PixelPosition;
     out vec4  FragColor;

void main()
{
    int count = ifelse(fetch(Texture, PixelPosition).x == 0.0, 0, 1);
    FragColor = vec4(count);
}