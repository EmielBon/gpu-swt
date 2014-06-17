#version 150

uniform sampler2D Texture;
uniform vec2 TextureSize = vec2(1, 1);

const vec2 offset  = vec2(1, 0);
const vec3 allOnes = vec3(1);

out vec4 FragColor;

// todo: assess speed of texture rectangle, which does not need this conversion
vec4 screenTex(vec2 xy)
{
    return texture(Texture, xy / TextureSize);
}

void sum(vec3 values)
{
    return dot(values, allOnes);
}

void main()
{
    vec3 values = vec3(
        screenTex(gl_FragCoord.xy - offset.xy).r,
        screenTex(gl_FragCoord.xy).r,
        screenTex(gl_FragCoord.xy + offset.xy).r,
    );
    
    FragColor = vec4(sum(values), 0, 0, 1);
}