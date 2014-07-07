#pragma include TextureUtil.fsh

uniform sampler2D Texture;

out vec4 FragColor;

flat in vec2 CurrentRootXY;

void main()
{
    ivec2 texSize = size(Texture);
    float x1 = float(texSize.x - 1) - CurrentRootXY.x;
    float y1 = float(texSize.y - 1) - CurrentRootXY.y;
    float x2 = CurrentRootXY.x;
    float y2 = CurrentRootXY.y;
    FragColor = vec4(x1, y1, x2, y2);
}