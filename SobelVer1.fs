#version 150

uniform sampler2D Texture;

const vec3 weights = vec3(3, 10, 3);

out vec4 FragColor;

vec4 screenTex(vec2 xy)
{
    return texture(Texture, xy / textureSize(Texture));
}

void main()
{
    vec3 result;
    float v = 0;
    
    result = vec3(
        screenTex(gl_FragCoord.xy + vec2(-1, 0)).r,
        screenTex(gl_FragCoord.xy + vec2( 0, 0)).r,
        screenTex(gl_FragCoord.xy + vec2( 1, 0)).r
    );
    
    v = dot(result, weights);
    FragColor = vec4(v, v, v, 1);
}