#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;

out vec4 FragColor;

flat in ivec2 PixelPosition;

void main()
{
    ivec2 current_xy      = PixelPosition;
    vec2  current_root_xy = fetch(Texture, PixelPosition).ba;
    
    if (current_xy != current_root_xy)
        discard;
    FragColor = vec4(1, 1, 1, 1);
}