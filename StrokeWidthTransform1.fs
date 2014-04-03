#version 150

uniform sampler2D Edges;
uniform sampler2D Gradients;

const float prec = 0.2;
const float Pi = 3.14159265359;
const float MaxOppositeEdgeGradientDifference = Pi / 2;
const float MaxRayLength = 50;
const vec4  Black = vec4(0, 0, 0, 1);
const int   MaxIterations = int(MaxRayLength / prec);

#define ditch { FragColor = Black; return; }

out vec4 FragColor;

vec4 fetch(sampler2D sampler, vec2 xy)
{
    return texelFetch(sampler, ivec2(xy), 0);
}

bool inRange(sampler2D sampler, vec2 xy)
{
    ivec2 size = textureSize(sampler, 0);
    return clamp(xy, ivec2(0), size) == xy;
}

bool isEdgePixel(vec2 xy)
{
    return (fetch(Edges, xy).r > 0.5);
}

void main()
{
    if (!isEdgePixel(gl_FragCoord.xy))
        ditch;
    
    vec2 dp = normalize( fetch(Gradients, gl_FragCoord.xy).xy );
    
    vec2 realPosition = gl_FragCoord.xy + vec2(0.5);
    ivec2 position = ivec2(realPosition);
    
    for (int i = 0; i <= MaxIterations + 1; ++i)
    {
        realPosition += dp * prec;
        position = ivec2(realPosition);
        if (!inRange(Edges, position) || i > MaxIterations)
            ditch;
        if (isEdgePixel(position))
            break;
    }
    
    vec2 dq = normalize( fetch(Gradients, position).xy );
    float rayLength = distance(gl_FragCoord.xy, position);
    int keep = int(acos(dot(dp, -dq)) < MaxOppositeEdgeGradientDifference);
    FragColor = vec4(vec3(rayLength * keep), 1);
}