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
    float result = 0;
    
    // Voor optimalisatie, misschien met dot doen
    result +=  1 * screenTex(FragTexCoord + vec2(0,-1)).r;
    result += 10 * screenTex(FragTexCoord + vec2(0, 0)).r;
    result +=  1 * screenTex(FragTexCoord + vec2(0, 1)).r;
    
    FinalColor = vec4(result, result, result, 1);
}