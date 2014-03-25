#version 150

uniform sampler2D Texture;
uniform vec2 TextureSize = vec2(1, 1);

out vec4 FragColor;

const float offsets[3] = float[](0.0, 1.3846153846, 3.2307692308);
const float weights[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

// todo: assess speed of texture rectangle, which does not need this conversion
vec4 screenTex(vec2 xy)
{
    return texture(Texture, xy / TextureSize);
}

void main()
{
    FragColor = screenTex(gl_FragCoord.xy) * weights[0];
    
    for(int i = 1; i < 3; ++i)
    {
        vec2 offset = vec2(offsets[i], 0);
        FragColor += screenTex(gl_FragCoord.xy + offset) * weights[i];
        FragColor += screenTex(gl_FragCoord.xy - offset) * weights[i];
    }
}