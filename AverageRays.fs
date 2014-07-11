#pragma include Util.fsh
#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D OppositePositions;
uniform sampler2D Values;

out vec4 FragColor;

void main()
{
    ivec2 pos0    = ivec2(gl_FragCoord.xy);
    float pos1_id = fetch(OppositePositions, pos0).r;
    // if pos1_id is 0.0 the pixel was either not an edge pixel, or the ray did not hit an edge pixel. In that case set pos0 and pos1 to the same value
    ivec2 pos1    = ifelse(pos1_id == 0.0, pos0, decode(pos1_id));
    
    bool steep = abs(pos1.y - pos0.y) > abs(pos1.x - pos0.x);
    
    pos0 = swizzleIf(steep, pos0);
    pos1 = swizzleIf(steep, pos1);
    
    ivec2 temp = pos0;
    bool rightToLeft = pos0.x > pos1.x;
    pos0 = ifelse(rightToLeft, pos1, pos0);
    pos1 = ifelse(rightToLeft, temp, pos1);
    
    pos0.x += 1;
    pos1 -= ivec2(1);
    
    int dx    = pos1.x - pos0.x;
    int dy    = abs(pos1.y - pos0.y);
    int err   = dx / 2;
    int ystep = ifelse(pos0.y < pos1.y, 1, -1);
    int y     = pos0.y;
    
    float sum = 0;
    int count = 0;
    
    for (int x = pos0.x; x <= pos1.x; ++x)
    {
        ivec2 pos = ivec2(x, y);
        
        float dSum = fetch(Values, swizzleIf(steep, pos)).r;
        
        sum += dSum;
        
        err = err - dy;
        
        count += ifelse(dSum == 0.0, 0, 1);
        
        y   += ifelse(err < 0, ystep, 0);
        err += ifelse(err < 0,    dx, 0);
    }
    
    //FragColor = vec4(ifelse(count < 1, 0, sum / count), debug / 10.0, 0, 1);
    float total = ifelse(count > 0, sum / count, 0);
    FragColor = vec4(total, 0, 0, 1);
}