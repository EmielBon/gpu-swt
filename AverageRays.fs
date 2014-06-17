#version 150

#pragma include Util.fsh
#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D OppositePositions;
uniform sampler2D Values;

out vec4 FragColor;

void main()
{
    ivec2 pos0 = ivec2(gl_FragCoord.xy);
    
    float pos1_id = fetch(OppositePositions, pos0).r;
    if (pos1_id == 0.0)
        discard;
    ivec2 pos1 = decode(pos1_id);
    
    bool steep = abs(pos1.y - pos0.y) > abs(pos1.x - pos0.x);
    
    pos0 = swizzleIf(steep, pos0);
    pos1 = swizzleIf(steep, pos1);
    
    ivec2 temp = pos0;
    bool rightToLeft = pos0.x > pos1.x;
    pos0 = ifelse(rightToLeft, pos1, pos0);
    pos1 = ifelse(rightToLeft, temp, pos1);
    
    int dx    = pos1.x - pos0.x;
    int dy    = abs(pos1.y - pos0.y);
    int err   = dx / 2;
    int ystep = ifelse(pos0.y < pos1.y, 1, -1);
    int y     = pos0.y;
    
    float sum = 0;
    float debug = 0;
    
    for (int x = pos0.x; x <= pos1.x; ++x)
    {
        ivec2 pos = ivec2(x, y);
        
        float dSum = fetch(Values, swizzleIf(steep, pos)).r;
        sum += dSum;
        
        err = err - dy;
        
        debug += ifelse(dSum == 0, 1, 0);
        
        y   += ifelse(err < 0, ystep, 0);
        err += ifelse(err < 0,    dx, 0);
    }
    
    FragColor = vec4(sum / dx, debug / 10, 0, 1);
}