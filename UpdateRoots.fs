#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
    ivec2 current_xy = ivec2(gl_FragCoord.xy);
    float current_id = 0.0;
    
    while(true)
    {
        current_id = fetch(Texture, current_xy).a;
        ivec2 root_xy = decode(current_id);
        if (root_xy == current_xy)
            break;
        current_xy = root_xy;
    }
    
    FragColor = vec4(0, 0, 0, current_id);
}