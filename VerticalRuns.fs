#version 150

#pragma include TextureUtil.fsh
#pragma include Codec.fsh

uniform sampler2D Texture;
uniform int       PassIndex;

out vec4 FragColor;

void main()
{
    ivec2 current_xy = ivec2(gl_FragCoord.xy);
    float current_id = fetch(Texture, current_xy).a;
    float value;
    if (PassIndex == 0)
    {
        ivec2 neighbor_xy = current_xy + ivec2(0, 1);
        float neighbor_id = fetch(Texture, neighbor_xy).a;
        //value = neighbor * clamp(pixel, 0, 1); // I get the idea, neighbour = [value|0], pixel = [0|value], so clamp(pixel) is probably [0|1], but it doesn't work
        value = (neighbor_id != 0 && current_id != 0) ? neighbor_id : current_id;
    }
    else
    {
        float next = fetch(Texture, decode(current_id) ).a;
        value = next;
    }
    
    FragColor = vec4(0, 0, 0, value);
}