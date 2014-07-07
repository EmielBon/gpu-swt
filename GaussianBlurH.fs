#pragma include TextureUtil.fsh

uniform sampler2D Texture;

out vec4 FragColor;

const float offsets[3] = float[](0.0, 1.3846153846, 3.2307692308);
const float weights[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main()
{
    vec2 pos = vec2(gl_FragCoord.xy);
    
    FragColor = sample(Texture, pos) * weights[0];
    
    for(int i = 1; i < 3; ++i)
    {
        vec2 offset = vec2(0, offsets[i]);
        FragColor += sample(Texture, pos + offset) * weights[i];
        FragColor += sample(Texture, pos - offset) * weights[i];
    }
}