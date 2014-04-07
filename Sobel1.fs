#version 150

uniform sampler2D Texture;

const vec3 weights = vec3(3, 10, 3);

out vec4 FragColor;

vec4 fetch(sampler2D sampler, vec2 xy)
{
    return texelFetch(sampler, ivec2(xy), 0);
}

void main()
{
    vec2 offset = vec2(0, 1);
    vec3 hResult, vResult;
    
    hResult[0] = fetch(Texture, gl_FragCoord.xy - offset.xy).r;
    vResult[0] = fetch(Texture, gl_FragCoord.xy - offset.yx).r;
    
    // Can probably prefetch this one if not using normal FragTexCoord, not gl_FragCoord + conversion
    hResult[1] = fetch(Texture, gl_FragCoord.xy).r;
    vResult[1] = fetch(Texture, gl_FragCoord.xy).r;
    
    hResult[2] = fetch(Texture, gl_FragCoord.xy + offset.xy).r;
    vResult[2] = fetch(Texture, gl_FragCoord.xy + offset.yx).r;
    
    float vHor = dot(hResult, weights);
    float vVer = dot(vResult, weights);
    
    FragColor = vec4(vHor, vVer, 0, 1);
}