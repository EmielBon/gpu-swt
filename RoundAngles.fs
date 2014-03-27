#version 150

uniform sampler2D Gradients;
uniform vec2 TextureSize = vec2(1, 1);

in  vec2 FragTexCoord;
out vec4 FragColor;

const float Pi = 3.14159265359;

vec4 screenTex(vec2 xy)
{
    return texture(Gradients, xy / TextureSize);
}

void main()
{
    vec2 gradient = texture(Gradients, FragTexCoord).xy;
    float G = length(gradient);
    gradient = normalize(gradient);
    
    float angle = atan(gradient.y, gradient.x); // range [-pi, pi]
    if (angle < 0)
        angle = atan(-gradient.y, -gradient.x);
    
    float binned = 0;
    
    if (angle < Pi / 8)
    {
        binned = 0;
    }
    else if (angle < 3 * Pi / 8)
    {
        binned = 1.0/4.0 * Pi;
    }
    else if (angle < 5 * Pi / 8)
    {
        binned = 1.0/2.0 * Pi;
    }
    else if (angle < 7 * Pi / 8)
    {
        binned = 3.0/4.0 * Pi;
    }
    
    vec2 binnedGradient = vec2(cos(binned), sin(binned));
    vec2 dir = vec2(sign(binnedGradient.x), sign(binnedGradient.y));
    
    if (G > length(screenTex(gl_FragCoord.xy + dir)) && G > length(screenTex(gl_FragCoord.xy - dir)))
        FragColor = vec4(1, 1, 1, 1);
    else
        FragColor = vec4(0, 0, 0, 1);
}