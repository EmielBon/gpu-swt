#version 150

#pragma include TextureUtil.fsh

uniform sampler2D Gradients;
uniform sampler2D LineLengths;
uniform sampler2D Values;
uniform bool      DarkOnLight;

out vec4 FragColor;

void main()
{
    vec2 gradient = normalize( fetch(Gradients, gl_FragCoord.xy).xy );
    float lineLength = fetch(LineLengths, gl_FragCoord.xy).r;
    ivec2 pos0 = ivec2(gl_FragCoord.xy);
    ivec2 pos1 = ivec2(pos0 + gradient * (lineLength * (DarkOnLight ? 1 : -1)));
    
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
    
    for (int x = pos0.x; x <= pos1.x; ++x)
    {
        vec2 pos = vec2(x, y);
        
        if (steep)
            sum += fetch(Values, pos.yx).r;
        else
            sum += fetch(Values, pos.xy).r;
        err = err - dy;
        
        if (err < 0) { y += ystep; err += dx; }
    }
    
    FragColor = vec4(vec3(sum / dx), 1);
}