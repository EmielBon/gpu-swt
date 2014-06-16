#version 150

#pragma include TextureUtil.fsh

uniform sampler2D Texture;
uniform float MinAspectRatio;
uniform float MaxAspectRatio;
uniform float MinSizeRatio;
uniform float MaxSizeRatio;

in  vec2 FragTexCoord;
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
    
    float aspectRatio = dims.x / dims.y;
    //float mean = GetMeanStrokeWidth();
    //float occupancy = GetOccupancy();
    //float variance = GetStrokeWidthVariance();
    
    float sizeRatio = area(dims) / area( size(Texture) );
    if (aspectRatio >= MinAspectRatio && aspectRatio <= MaxAspectRatio && sizeRatio >= MinSizeRatio && sizeRatio <= MaxSizeRatio)
        FragColor = bbox;
    else
        FragColor = vec4(0, 0, 0, 0);
}