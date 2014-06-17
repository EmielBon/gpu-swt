#version 150

uniform sampler2D Gradients;
uniform vec2 TextureSize = vec2(1, 1);
uniform float LowerThreshold = 0.10;
uniform float UpperThreshold = 0.30;

out vec4 FragColor;

const float Pi = 3.14159265359;
const float PiOver2 = Pi / 2;
const float PiOver4 = Pi / 4;
const float PiOver8 = Pi / 8;

vec4 screenTex(vec2 uv)
{
    return texture(Gradients, uv / TextureSize);
}

void main()
{
    vec2 gradient = screenTex(gl_FragCoord.xy).rg;
    
    float angle = gradient.x;
    float magnitude = gradient.y;
    
    vec4 a = vec4(PiOver8, 3 * PiOver8, 5 * PiOver8, 7 * PiOver8);
    vec4 b = vec4(angle);
    vec4 r = step(a, b);
    float binned = dot(r, vec4(PiOver4));
    
    // todo: assumes binned == Pi/2 -> cos(binned) == 0 (anaologuous for sin), hopefully glsl is that precise (should test)
    vec2 binnedGradient = round( vec2(cos(binned), sin(binned)) );
    vec2 dir = sign(binnedGradient);
    
    float  forwardNeighbourGradient = screenTex(gl_FragCoord.xy + dir).g;
    float backwardNeighbourGradient = screenTex(gl_FragCoord.xy - dir).g;
    
    bool localMaximum = magnitude > forwardNeighbourGradient && magnitude > backwardNeighbourGradient;
    vec2 constants = vec2(1.0, localMaximum);
    
    FragColor = constants.xxxx * constants.yyyx;
}