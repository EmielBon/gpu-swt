#pragma include TextureUtil.fsh

uniform sampler2D StrokeWidths;
uniform sampler2D PixelCounts;
uniform sampler2D Averages;

flat in  ivec2 CurrentXY;
flat in  ivec2 CurrentRootXY;
     out vec4  FragColor;

void main()
{
    float strokeWidth        = -(fetch(StrokeWidths, CurrentXY).r - 1000);
    float count              =   fetch(PixelCounts, CurrentRootXY).r;
    float averageStrokeWidth = -(fetch(Averages, CurrentRootXY).a - 1000);
    
    float stdDev   = strokeWidth - averageStrokeWidth; // standard deviation
    float variance = stdDev * stdDev; // variance is standard deviation ^ 2
    // todo: maybe too small?
    FragColor = vec4(variance, 0, 0, 1);
}