#pragma include Util.fsh
#pragma include TextureUtil.fsh

uniform sampler2D PixelCounts;
uniform sampler2D InputImage;
uniform sampler2D StrokeWidths;

flat in  ivec2 CurrentXY;
flat in  ivec2 CurrentRootXY;
     out vec4  FragColor;

void main()
{
    float count       = fetch(PixelCounts,  CurrentRootXY).r;
    vec3  color       = fetch(InputImage,   CurrentXY).rgb;
    float strokeWidth = fetch(StrokeWidths, CurrentXY).r;

    FragColor = vec4(color, strokeWidth) / count;
}