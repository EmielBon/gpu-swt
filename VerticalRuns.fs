#version 150

#pragma include TextureUtil.fsh
#pragma include Codec.fsh

uniform sampler2D Texture;
uniform int PassIndex;

out vec4 FragColor;

float getId(ivec2 uv_coord)
{
    return fetch(Texture, uv_coord).z;
}

void main()
{
    ivec2 uv_coord = ivec2(gl_FragCoord.xy);
    float pixel = getId(uv_coord);
    float value;
    if (PassIndex == 0)
    {
        ivec2 uv = uv_coord + ivec2(0, 1);
        float neighbor = getId(uv);
        //value = neighbor * clamp(pixel, 0, 1); // I get the idea, neighbour = [value|0], pixel = [0|value], so clamp(pixel) is probable [0|1], but it doesn't work
        value = (neighbor != 0 && pixel != 0) ? neighbor : pixel;
    }
    else
    {
        float next = getId( decode(pixel) );
        value = next;
    }
    FragColor = vec4(decode(value) / vec2(textureSize(Texture, 0)), value, 1);
}