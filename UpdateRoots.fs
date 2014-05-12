#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
    ivec2 current_xy = ivec2(gl_FragCoord.xy);
    
    float root_id = 0.0;
    while(true)
    {
        root_id = fetch(Texture, current_xy).z;
        if (root_id == 0.0)
        {
            discard;
        }
        ivec2 root_xy = decode(root_id);
        if (root_xy == current_xy)
            break;
        current_xy = root_xy;
    }
    FragColor = vec4(decode(root_id) / vec2(textureSize(Texture, 0)), root_id, 1);
}