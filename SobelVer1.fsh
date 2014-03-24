#version 150

uniform sampler2D Texture;
uniform vec2 TextureSize = vec2(1, 1);

in  vec2 FragTexCoord;
out vec4 FinalColor;

const vec3 weights = vec3(3, 10, 3);

vec4 screenTex(vec2 xy)
{
    return texture(Texture, xy / TextureSize);
}

void main()
{
    vec3 result;
    float v = 0;
    
    result = vec3(
        screenTex(FragTexCoord + vec2(-1, 0)).r,
        screenTex(FragTexCoord + vec2( 0, 0)).r,
        screenTex(FragTexCoord + vec2( 1, 0)).r
    );
    
    v = dot(result, weights);
    FinalColor = vec4(v, v, v, 1);
}