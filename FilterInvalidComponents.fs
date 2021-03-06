#pragma include Util.fsh
#pragma include TextureUtil.fsh

const float MinHeight = 10;
const float MaxHeight = 300;
const float MinAspectRatio = 0.1; // Paper says 0.1
const float MaxAspectRatio = 2; // Paper says 10, but this allows for very short, broad shapes which are generally not letters
const float MinOccupancy = 0.2;
const float MaxOccupancy = 0.9;
const float MaxVarianceToMeanRatio = 1.5;

uniform sampler2D BoundingBoxes;
uniform sampler2D Averages;
uniform sampler2D PixelCounts;
uniform sampler2D VarianceSums;

out vec4 FragColor;

float getArea(vec2 dimensions)
{
    return dimensions.x * dimensions.y;
}

int getArea(ivec2 dimensions)
{
    return dimensions.x * dimensions.y;
}

void main()
{
    ivec2 current_xy = ivec2(gl_FragCoord.xy);
    ivec2 texSize    = size(BoundingBoxes);
    
    vec4  bbox      = fetch(BoundingBoxes, current_xy);
    float mean      = fetch(Averages, current_xy).a;

    float x1 = abs(bbox.x - float(texSize.x - 1));
    float y1 = abs(bbox.y - float(texSize.y - 1));
    float x2 = bbox.z;
    float y2 = bbox.w;
    
    vec2 dims = vec2(x2 - x1, y2 - y1);
    
    float area       = getArea(dims);
    float pixelCount = fetch(PixelCounts, current_xy).r;
    float occupancy  = pixelCount / pixelCount;
    float variance   = fetch(VarianceSums, current_xy).r / pixelCount;
    
    float aspectRatio   = dims.x / dims.y;
    float sizeRatio     = area / getArea(texSize);
    bool  goodAspect    = aspectRatio >= MinAspectRatio && aspectRatio <= MaxAspectRatio;
    bool  goodSize      = dims.y >= MinHeight && dims.y <= MaxHeight && area > 64;
    bool  goodOccupancy = occupancy >= MinOccupancy && (aspectRatio < 1 || occupancy <= MaxOccupancy);
    bool  goodVariance  = variance <= mean / MaxVarianceToMeanRatio;
    
    FragColor = ifelse(goodAspect && goodSize && goodOccupancy && goodVariance, bbox, vec4(0));
}