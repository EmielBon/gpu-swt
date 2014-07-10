#pragma include TextureUtil.fsh

uniform sampler2D StrokeWidths;
uniform sampler2D Occupancy;
uniform sampler2D Averages;

flat in  ivec2 CurrentXY;
flat in  ivec2 CurrentRootXY;
     out vec4  FragColor;

void main()
{
    float strokeWidth        = fetch(StrokeWidths, CurrentXY).r;
    float count              = fetch(Occupancy, CurrentRootXY).r;
    float averageStrokeWidth = fetch(Averages, CurrentRootXY).a;
    
    float stdDev   = strokeWidth - averageStrokeWidth; // standard deviation
    float variance = stdDev * stdDev; // variance is standard deviation ^ 2
    // todo: maybe too small?
    FragColor = vec4(variance /*/ count*/, 0, 0, 1);
}