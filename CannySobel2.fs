#version 150

uniform sampler2D Texture;

const vec2 weights = vec2(-1,/*0,*/1);

out vec4 FragColor;

// todo: assess speed of texture rectangle, which does not need this conversion
vec4 screenTex(vec2 xy)
{
    return texelFetch(Texture, ivec2(xy), 0);
    //return texture(Texture, xy / textureSize(Texture));
}

float atan2(vec2 v)
{
    return atan(v.y, v.x);
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
    
    // Gradients can potentially range between [-16, 16] with the Scharr operator, this normalizes that range to [-1, 1]
    vec2 gradient = vec2(vHor, vVer) / 16;
    float angle = max(atan2(gradient), atan2(-gradient));
    
    FragColor = vec4(angle, length(gradient), 0, 1);
}