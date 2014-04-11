#version 150

#pragma include TextureUtil.fsh

uniform sampler2D Gradients;
uniform float LowerThreshold = 0.08;
uniform float UpperThreshold = 0.16;

in  vec2 FragTexCoord;
out vec4 FragColor;

const float Pi = 3.14159265359;
const float PiOver2 = Pi / 2;
const float PiOver4 = Pi / 4;
const float PiOver8 = Pi / 8;

void main()
{
    vec2 gradient = fetch(Gradients, gl_FragCoord.xy).rg;
    
    float angle = gradient.x;
    float magnitude = gradient.y;
    
    vec4 a = vec4(PiOver8, 3 * PiOver8, 5 * PiOver8, 7 * PiOver8);
    vec4 b = vec4(angle);
    vec4 r = step(a, b);
    float binned = dot(r, vec4(PiOver4));
    
    // todo: is round implementation dependant?
    vec2 binnedGradient = round( vec2(cos(binned), sin(binned)) );
    vec2 dir = sign(binnedGradient);
    
    float  forwardNeighbourGradient = fetch(Gradients, gl_FragCoord.xy + dir).g;
    float backwardNeighbourGradient = fetch(Gradients, gl_FragCoord.xy - dir).g;
    
    float sum = 0;
    // todo: seprarable convolution (averaging) operation
    for (int i = -1; i <= 1; ++i)
    for (int j = -1; j <= 1; ++j)
        // todo: look at lerp instead of smoothstep
        sum += smoothstep(LowerThreshold, UpperThreshold, fetch(Gradients, gl_FragCoord.xy + vec2(i, j)).g);
    
    bool localMaximum = magnitude > forwardNeighbourGradient && magnitude > backwardNeighbourGradient;
    bool strongEdge = magnitude > UpperThreshold;
    bool weakEdge = magnitude >= LowerThreshold;
    bool acceptedWeakEdge = weakEdge && sum / 9 >= UpperThreshold;
    bool isEdgePixel = localMaximum && (strongEdge || acceptedWeakEdge);
    
    if (!isEdgePixel)
        discard; // used to directly build the stencil buffer
    else
        FragColor = vec4( vec3(isEdgePixel ? 1 : 0), 0 );
}