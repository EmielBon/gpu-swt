#pragma include Util.fsh
#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Edges;
uniform sampler2D Gradients;
uniform bool      DarkOnLight;

const lowp    float prec = 0.2;
const float         Pi   = 3.14159265359;
const float         MaxOppositeEdgeGradientDifference = Pi / 2;
const lowp    int   MaxRayLength = 50; // todo: somehow setting this to 50 messes everything up
const lowp    vec4  Black = vec4(0, 0, 0, 1);
const lowp    int   MaxIterations = int(MaxRayLength / prec); // todo: misschien beter mediump als dit nog groter wordt

out vec4 FragColor;

bool inRange(ivec2 xy)
{
    // from 1,1 tot w-1,h-1 because edge pixels are often incorrectly classified
    return clamp(xy, ivec2(1), size(Edges) - ivec2(2)) == xy;
}

// todo: maybe optimize in the same way as gaussian, by sampling in between pixels instead of fetching
bool isEdgePixel(ivec2 xy)
{
    return fetch(Edges, xy).r == 1.0;
}

void main()
{
    ivec2 pos0 = ivec2(gl_FragCoord.xy);
    // todo: choose better variable name
    lowp int bla = ifelse(DarkOnLight, 1, -1);
    
    // todo: should not be needed with stencil test, but removing it did not function as expected (should retest)
    if (!isEdgePixel(pos0))
    {
        FragColor = Black;
        return;
    }
    
    vec2 gradient0 = fetch(Gradients, pos0).xy;
    vec2 dir       = normalize(gradient0) * bla;
    vec2 dp        = dir * prec;
    
    vec2  realPos1 = pos0 + vec2(0.5) + dir;
    ivec2 pos1     = ivec2(realPos1);
    
    bool found = false;
    
    // todo: maybe optimize in the same way as gaussian, by sampling in between pixels instead of fetching
    for (int i = 0; i <= MaxIterations; ++i)
    {
        realPos1 += dp * int(!found);
        pos1      = ivec2(realPos1);
        found     = found || isEdgePixel(pos1);
    }
    
    vec2 gradient1 = fetch(Gradients, pos1).xy;
    vec2 dq = normalize(gradient1) * bla;
    float edgeDifference = acos(dot(dp, -dq));
    bool keep = found && inRange(pos0) && inRange(pos1) && edgeDifference < MaxOppositeEdgeGradientDifference;
    FragColor = vec4(encode(pos1) * int(keep), 0, 0, 1);
}