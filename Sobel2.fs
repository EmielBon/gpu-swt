#version 150

uniform sampler2D Texture;
uniform vec2 TextureSize = vec2(1, 1);

const vec2 weights = vec2(-1,/*0,*/1);

out vec4 FragColor;

// todo: assess speed of texture rectangle, which does not need this conversion
vec4 screenTex(vec2 xy)
{
    return texture(Texture, xy / TextureSize);
}

void main()
{
    vec2 offset = vec2(0, 1);
    vec2 hResult, vResult;
    
    hResult[0] = screenTex(gl_FragCoord.xy - offset.yx).r;
    vResult[0] = screenTex(gl_FragCoord.xy - offset.xy).g;
    
    hResult[1] = screenTex(gl_FragCoord.xy + offset.yx).r;
    vResult[1] = screenTex(gl_FragCoord.xy + offset.xy).g;
    
    float vHor = dot(hResult, weights);
    float vVer = dot(vResult, weights);
    
    FragColor = vec4(vHor, vVer, 0, 1);
}