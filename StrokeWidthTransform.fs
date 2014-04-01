#version 150

uniform sampler2D Texture;
uniform vec2 TextureSize = vec2(1, 1);

const vec3 weights = vec3(3, 10, 3);

out vec4 FragColor;

// todo: assess speed of texture rectangle, which does not need this conversion
vec4 screenTex(vec2 xy)
{
    return texelFetch(Texture, xy, 0);
    //return texture(Texture, xy / textureSize(Texture));
}

void main()
{
    vec2 offset = vec2(0, 1);
    vec3 hResult, vResult;
    
    hResult[0] = screenTex(gl_FragCoord.xy - offset.xy).r;
    vResult[0] = screenTex(gl_FragCoord.xy - offset.yx).r;
    
    // Can probably prefetch this one if not using normal FragTexCoord, not gl_FragCoord + conversion
    hResult[1] = vResult[1] = screenTex(gl_FragCoord.xy).r;
    
    hResult[2] = screenTex(gl_FragCoord.xy + offset.xy).r;
    vResult[2] = screenTex(gl_FragCoord.xy + offset.yx).r;
    
    float vHor = dot(hResult, weights);
    float vVer = dot(vResult, weights);
    
    FragColor = vec4(vHor, vVer, 0, 1);
}