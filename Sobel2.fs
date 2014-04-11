#version 150

#pragma include TextureUtil.fsh

uniform sampler2D Texture;

const vec2 weights = vec2(-1,/*0,*/1);

out vec4 FragColor;

void main()
{
    vec2 offset = vec2(0, 1);
    vec2 hResult, vResult;
    
    hResult[0] = fetch(Texture, gl_FragCoord.xy - offset.yx).r;
    vResult[0] = fetch(Texture, gl_FragCoord.xy - offset.xy).g;
    
    hResult[1] = fetch(Texture, gl_FragCoord.xy + offset.yx).r;
    vResult[1] = fetch(Texture, gl_FragCoord.xy + offset.xy).g;
    
    float vHor = dot(hResult, weights);
    float vVer = dot(vResult, weights);
    
    FragColor = vec4(vHor, vVer, 0, 1);
}