#version 150

uniform sampler2D Texture;
uniform vec2 TextureSize = vec2(1, 1);

in  vec2 FragTexCoord;
out vec4 FinalColor;

vec4 screenTex(vec2 xy)
{
    return texture(Texture, xy / TextureSize);
}

void main()
{
    float result = 0.0;
    
    result +=  -1 * screenTex(FragTexCoord + vec2(-1, -1)).r;
    //result +=  0 * tex(FragTexCoord + screenCoord( 0, -1));
    result +=   1 * screenTex(FragTexCoord + vec2( 1, -1)).r;
    result += -10 * screenTex(FragTexCoord + vec2(-1,  0)).r;
    //result +=  0 * tex(FragTexCoord + screenCoord( 0,  0));
    result +=  10 * screenTex(FragTexCoord + vec2( 1,  0)).r;
    result +=  -1 * screenTex(FragTexCoord + vec2(-1,  1)).r;
    //result +=  0 * tex(FragTexCoord + screenCoord( 0,  1));
    result +=   1 * screenTex(FragTexCoord + vec2( 1,  1)).r;
    
    FinalColor = vec4(result, result, result, 1);
}