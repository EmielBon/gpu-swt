#pragma include TextureUtil.fsh

uniform sampler2D Texture;

const ivec2 weights = ivec2(-1,/*0,*/1);

out vec4 FragColor;

float atan2(vec2 v)
{
    return atan(v.y, v.x);
}

void main()
{
    vec2 pos = vec2(gl_FragCoord.xy);
    vec2 offset = vec2(0, 1);
    vec2 hResult, vResult;
    
    hResult[0] = fetch(Texture, pos - offset.yx).r;
    vResult[0] = fetch(Texture, pos - offset.xy).g;
    
    hResult[1] = fetch(Texture, pos + offset.yx).r;
    vResult[1] = fetch(Texture, pos + offset.xy).g;
    
    float vHor = dot(hResult, weights);
    float vVer = dot(vResult, weights);
    
    // Gradients can potentially range between [-16, 16] with the Scharr operator, this normalizes that range to [-1, 1]
    vec2 gradient = vec2(vHor, vVer) / /*16*/5;
    float angle = max(atan2(gradient), atan2(-gradient));
    
    FragColor = vec4(angle, length(gradient), 0, 1);
}