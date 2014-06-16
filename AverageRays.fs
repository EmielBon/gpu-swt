#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

//uniform sampler2D Gradients;
uniform sampler2D OppositePositions;
uniform sampler2D Values;
//uniform bool      DarkOnLight;

out vec4 FragColor;

void main()
{
    ivec2 pos0 = ivec2(gl_FragCoord.xy);
    
    //vec2 gradient = normalize( fetch(Gradients, pos0).xy );
    float pos1_id = fetch(OppositePositions, pos0).r;
    if (pos1_id == 0.0)
        discard;
    ivec2 pos1 = decode(pos1_id);//ivec2(pos0 + gradient * (lineLength * (DarkOnLight ? 1 : -1)));
    
    bool steep = abs(pos1.y - pos0.y) > abs(pos1.x - pos0.x);
    
    if (steep)
    {
        pos0.xy = pos0.yx;
        pos1.xy = pos1.yx;
    }
    if (pos0.x > pos1.x)
    {
        ivec2 temp = pos0;
        pos0 = pos1;
        pos1 = temp;
    }
    
    int dx = pos1.x - pos0.x;
    int dy = abs(pos1.y - pos0.y);
    int err = dx / 2;
    int ystep = (pos0.y < pos1.y ? 1 : -1);
    int y = pos0.y;
    
    float sum = 0;
    float debug = 0;
    
    for (int x = pos0.x; x <= pos1.x; ++x)
    {
        ivec2 pos = ivec2(x, y);
        
        float oldSum = sum;
        
        if (steep)
            sum += fetch(Values, pos.yx).r;
        else
            sum += fetch(Values, pos.xy).r;
        err = err - dy;
        
        if (oldSum == sum)
            debug += 1;
        
        if (err < 0) { y += ystep; err += dx; }
    }
    
    FragColor = vec4(sum / dx, debug / 10, 0, 1);
}