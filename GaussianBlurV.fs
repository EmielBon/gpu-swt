#version 150

#pragma include TextureUtil.fsh

uniform sampler2D Texture;

out vec4 FragColor;

const float offsets[3] = float[](0.0, 1.3846153846, 3.2307692308);
const float weights[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main()
{
    FragColor = sample(Texture, gl_FragCoord.xy) * weights[0];
    
    for(int i = 1; i < 3; ++i)
    {
        vec2 offset = vec2(offsets[i], 0);
        FragColor += sample(Texture, gl_FragCoord.xy + offset) * weights[i];
        FragColor += sample(Texture, gl_FragCoord.xy - offset) * weights[i];
    }
}