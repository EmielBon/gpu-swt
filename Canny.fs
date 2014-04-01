#version 150

uniform sampler2D Gradients;
uniform vec2 TextureSize = vec2(1, 1);
uniform float LowerThreshold = 0.10;
uniform float UpperThreshold = 0.30;

in  vec2 FragTexCoord;
out vec4 FragColor;

const float Pi = 3.14159265359;
const float PiOver2 = Pi / 2;
const float PiOver4 = Pi / 4;
const float PiOver8 = Pi / 8;

vec4 screenTex(vec2 uv)
{
    return texture(Gradients, uv / TextureSize);
}

float atan2(vec2 v)
{
    return atan(v.y, v.x);
}

vec2 getGradient(vec2 uv)
{
    // Gradients can potentially range between [-16, 16] with the Scharr operator, this normalizes that range to [-1, 1]
    return screenTex(uv).xy / 16;
}

void main()
{
    vec2 gradient = getGradient(gl_FragCoord.xy);
    float G = length(gradient);
    gradient = normalize(gradient);
    // range [-pi, pi]
    float angle = max(atan2(gradient), atan2(-gradient));
    
    float binned = 0;
    
    vec4 a = vec4(PiOver8, 3 * PiOver4, 5 * PiOver8, 7 * PiOver8);
    vec4 b = vec4(angle);
    vec4 r = step(a, b);
    binned = dot(r, vec4(PiOver4));
    
    /*if (angle < PiOver8)
        binned = 0;
    else if (angle < 3 * PiOver8)
        binned = PiOver4;
    else if (angle < 5 * PiOver8)
        binned = PiOver2;
    else if (angle < 7 * PiOver8)
        binned = 3 * PiOver4;*/
    
    vec2 binnedGradient = vec2(cos(binned), sin(binned));
    vec2 dir = sign(binnedGradient);
    
    vec2 forwardNeighbourGradient  = getGradient(gl_FragCoord.xy + dir);
    vec2 backwardNeighbourGradient = getGradient(gl_FragCoord.xy - dir);

    FragColor = vec4(0, 0, 0, 1);
    
    if (G < length(forwardNeighbourGradient) || G < length(backwardNeighbourGradient))
        FragColor = vec4(0, 0, 0, 1);
    else if (G > UpperThreshold)
        FragColor = vec4(1, 1, 1, 1);
    else if (G >= LowerThreshold)
    {
        float sum = 0;
        // todo: seprarable convolution (averaging) operation
        for (int i = -1; i <= 1; ++i)
        for (int j = -1; j <= 1; ++j)
        {
            if (i == 0 && j == 0)
                continue;
            sum += smoothstep(LowerThreshold, UpperThreshold, length(getGradient(gl_FragCoord.xy + vec2(i, j))));
        }
        
        if (sum >= 2)
            FragColor = vec4(1, 1, 1, 1);
    }
}