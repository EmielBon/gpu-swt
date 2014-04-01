#version 150

uniform sampler2D Texture;

out vec4 FragColor;

vec4 screenTex(vec2 xy)
{
    return texture(Texture, xy / textureSize(Texture));
}

void main()
{
    float result = 0.0;
    
    result +=  -1 * screenTex(gl_FragCoord.xy + vec2(-1, -1)).r;
    //result +=  0 * tex(gl_FragCoord.xy + screenCoord( 0, -1));
    result +=   1 * screenTex(gl_FragCoord.xy + vec2( 1, -1)).r;
    result += -10 * screenTex(gl_FragCoord.xy + vec2(-1,  0)).r;
    //result +=  0 * tex(gl_FragCoord.xy + screenCoord( 0,  0));
    result +=  10 * screenTex(gl_FragCoord.xy + vec2( 1,  0)).r;
    result +=  -1 * screenTex(gl_FragCoord.xy + vec2(-1,  1)).r;
    //result +=  0 * tex(gl_FragCoord.xy + screenCoord( 0,  1));
    result +=   1 * screenTex(gl_FragCoord.xy + vec2( 1,  1)).r;
    
    FragColor = vec4(result, result, result, 1);
}