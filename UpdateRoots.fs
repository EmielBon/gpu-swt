#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
    ivec2 current_xy = ivec2(gl_FragCoord.xy);
    float current_id  = 0.0;
    
    while(true)
    {
        current_id = fetch(Texture, current_xy).a;
        ivec2 root_xy = decode(current_id);
        if (all(equal(root_xy, current_xy)))
            break;
        current_xy = root_xy;
    }
    
    FragColor = vec4(0, 0, 0, current_id);
}

/*void main()
{
    ivec2 current_xy = ivec2(gl_FragCoord.xy);
    float current_id  = 0.0;
    float previous_id = 0.0;
    
    int counter1 = 0;
    int counter2 = 0;
    
    while(counter1++ < 10000)
    {
        current_id = fetch(Texture, current_xy).a;
        
        if (current_id == 0.0)
        {
            FragColor = vec4(0, 0, 0, 1);
            return;
        }
        
        if (current_id == previous_id && counter2++ > 1000)
        {
            FragColor = vec4(1, 0, 0, 1);
            return;
        }
        
        ivec2 root_xy = decode(current_id);
        if (all(equal(root_xy, current_xy)))
            break;
        current_xy = root_xy;
        
        previous_id = current_id;
    }
    
    FragColor = vec4(0, 0, 0, current_id);
}*/