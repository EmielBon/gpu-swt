#version 150

#pragma include TextureUtil.fsh

uniform sampler2D Edges;
uniform sampler2D Gradients;
uniform bool DarkOnLight;// = true;

const float prec = 0.2;
const float Pi = 3.14159265359;
const float MaxOppositeEdgeGradientDifference = Pi / 2;
const float MaxRayLength = 50;
const vec4  Black = vec4(0, 0, 0, 1);
const int   MaxIterations = int(MaxRayLength / prec);

#define ditch { FragColor = Black; return; }
#define ditch2 { FragColor = vec4(1, 0, 0, 1); return; }
#define ditch3 { FragColor = vec4(0, 1, 0, 1); return; }

out vec4 FragColor;

bool inRange(sampler2D sampler, ivec2 xy)
{
    ivec2 size = textureSize(sampler, 0);
    return clamp(xy, ivec2(0), size) == xy;
}

bool isEdgePixel(vec2 xy)
{
    return fetch(Edges, xy).r == 1.0;
}

void main()
{
    if (!isEdgePixel(gl_FragCoord.xy))
        ditch;
    
    vec2 gradient = fetch(Gradients, gl_FragCoord.xy).xy;
    vec2 dp = normalize(gradient) * (prec * (DarkOnLight ? 1 : -1));
    
    vec2 realPosition = gl_FragCoord.xy + vec2(0.5);
    ivec2 position = ivec2(realPosition);
    
    for (int i = 0; i <= MaxIterations; ++i)
    {
        if (i == MaxIterations)
            ditch;
        realPosition += dp;
        ivec2 newPos = ivec2(realPosition);
        if (newPos == position)
            continue;
        position = newPos;
        if (!inRange(Edges, position))
            ditch;
        if (isEdgePixel(position) && distance(gl_FragCoord.xy, position) > 5)
            break;
    }
    
    vec2 dq = normalize( fetch(Gradients, position).xy ) * (DarkOnLight ? 1 : -1);
    float rayLength = distance(gl_FragCoord.xy, position);
    int keep = int(acos(dot(dp, -dq)) < MaxOppositeEdgeGradientDifference);
    FragColor = vec4(vec3(rayLength * keep), 1);
    //if (rayLength < 5)
    //    FragColor = vec4(0, 0, 1, 1);
}