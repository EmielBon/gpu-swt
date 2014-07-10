#pragma include TextureUtil.fsh

uniform sampler2D Components;

flat in  ivec2 CurrentXY;
     out vec4  FragColor;

void main()
{
    ivec2 texSize = size(Components);
    float x1 = float(texSize.x - 1) - CurrentXY.x;
    float y1 = float(texSize.y - 1) - CurrentXY.y;
    float x2 = CurrentXY.x;
    float y2 = CurrentXY.y;
    FragColor = vec4(x1, y1, x2, y2);
}