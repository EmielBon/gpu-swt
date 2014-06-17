#version 150

#pragma include Util.fsh
#pragma include Codec.fsh
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

out vec4 FragColor;

bool inRange(ivec2 xy)
{
    return clamp(xy, ivec2(0), size(Edges)) == xy;
}

bool isEdgePixel(ivec2 xy)
{
    return fetch(Edges, xy).r == 1.0;
}

void main()
{
    ivec2 pos0 = ivec2(gl_FragCoord.xy);
    int   dir  = ifelse(DarkOnLight, 1, -1);
    float meh  = dir * prec;
    
    // todo: should not be needed with stencil test, but removing it does not function as expected
    if (!isEdgePixel(pos0))
    {
        FragColor = Black;
        return;
    }
    
    vec2 gradient0 = fetch(Gradients, pos0).xy;
    vec2 dp = normalize(gradient0) * meh;
    
    vec2 realPos1 = pos0 + vec2(0.5) + (dp * 5);
    ivec2 pos1 = ivec2(realPos1);
    
    bool found = false;
    
    for (int i = 0; i <= MaxIterations; ++i)
    {
        realPos1 += dp * int(!found);
        pos1      = ivec2(realPos1);
        found     = found || isEdgePixel(pos1);
    }
    
    //vec2 gradient1 = fetch(Gradients, pos1).xy;
    //vec2 dq = normalize(gradient1) * dir;
    //int keep = int(acos(dot(dp, -dq)) < MaxOppositeEdgeGradientDifference);
    // todo: include above
    bool keep = found && inRange(pos1);
    FragColor = vec4(encode(pos1) * int(keep), 0, 0, 1);
}