#pragma include Util.fsh
#pragma include Codec.fsh
#pragma include SWTUtil.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Encoded;
uniform sampler2D StrokeWidths;
uniform int       PassIndex;

out vec4 FragColor;

void main()
{
    ivec2 current_xy = ivec2(gl_FragCoord.xy);
    float current_id = fetch(Encoded, current_xy).a;
    float value;
    if (PassIndex == 0)
    {
        ivec2 neighbor_xy = current_xy + ivec2(0, 1);
        float neighbor_id = fetch(Encoded, neighbor_xy).a;
        //value = neighbor * clamp(pixel, 0, 1); // I get the idea, neighbour = [value|0], pixel = [0|value], so clamp(pixel) is probably [0|1], but it doesn't work
        float current_sw  = fetch(StrokeWidths, current_xy).r;
        float neighbor_sw = fetch(StrokeWidths, neighbor_xy).r;
        value = ifelse(neighbor_id != 0 && current_id != 0 && getRatio(current_sw, neighbor_sw) <= 3.0, neighbor_id, current_id);
    }
    else
    {
        float next = fetch(Encoded, decode(current_id) ).a;
        value = next;
    }
    
    FragColor = vec4(0, 0, 0, value);
}