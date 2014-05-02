#version 150

#pragma include TextureUtil.fsh

uniform sampler2D Edges;
uniform sampler2D Gradients;
uniform bool      DarkOnLight;

const float prec = 0.2;
const float Pi = 3.14159265359;
const float MaxOppositeEdgeGradientDifference = Pi / 2;
const int   MaxRayLength = 50;
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

bool isEdgePixel(ivec2 xy)
{
    return fetch(Edges, xy).r == 1.0;
}

void main()
{
    ivec2 pos0 = ivec2(gl_FragCoord.xy);
    int dir = DarkOnLight ? 1 : -1;
    float meh = dir * prec;
    
    if (!isEdgePixel(pos0))
        ditch;
    
    vec2 gradient0 = fetch(Gradients, pos0).xy;
    vec2 dp = normalize(gradient0) * meh;
    
    vec2 realPos1 = pos0 + vec2(0.5) + (dp * 5);
    ivec2 pos1 = ivec2(realPos1);
    
    int i;
    for (i = 0; i <= MaxIterations; ++i)
    {
        realPos1 += dp;
        //ivec2 newPos1 = ivec2(realPos1);
        //if (newPos1 == pos1)
        //    continue;
        //pos1 = newPos1;
        pos1 = ivec2(realPos1);
        if (isEdgePixel(pos1))
            break;
    }
    
    if (i >= MaxIterations || !inRange(Edges, pos1))
        ditch;
    
    vec2 gradient1 = fetch(Gradients, pos1).xy;
    vec2 dq = normalize(gradient1) * dir;
    float rayLength = distance(gl_FragCoord.xy, realPos1);
    int keep = int(acos(dot(dp, -dq)) < MaxOppositeEdgeGradientDifference);
    FragColor = vec4(vec3(rayLength * keep), 1);
    //if (rayLength < 5)
    //    FragColor = vec4(0, 0, 1, 1);
}