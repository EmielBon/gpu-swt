#pragma include Util.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;
uniform float MinAspectRatio;
uniform float MaxAspectRatio;
uniform float MinSizeRatio;
uniform float MaxSizeRatio;

out vec4 FragColor;

float area(vec2 dimensions)
{
    return dimensions.x * dimensions.y;
}

void main()
{
    ivec2 texSize = size(Texture);
    vec4 bbox = fetch(Texture, gl_FragCoord.xy);
    
    float x1 = abs(bbox.x - float(texSize.x - 1));
    float y1 = abs(bbox.y - float(texSize.y - 1));
    float x2 = bbox.z;
    float y2 = bbox.w;
    
    vec2 dims = vec2(x2 - x1, y2 - y1);
    
    //float mean = GetMeanStrokeWidth();
    //float occupancy = GetOccupancy();
    //float variance = GetStrokeWidthVariance();
    
    float aspectRatio = dims.x / dims.y;
    float sizeRatio   = area(dims) / area( size(Texture) );
    bool  goodAspect  = aspectRatio >= MinAspectRatio && aspectRatio <= MaxAspectRatio;
    bool  goodSize    = sizeRatio >= MinSizeRatio && sizeRatio <= MaxSizeRatio;
    
    FragColor = ifelse(goodAspect && goodSize, bbox, vec4(0));
}