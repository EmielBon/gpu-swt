#version 150

uniform sampler2D Texture;

const vec2 weights = vec2(-1,/*0,*/1);

out vec4 FragColor;

vec4 screenTex(vec2 xy)
{
    return texture(Texture, xy / textureSize(Texture));
}

void main()
{
    vec2 result;
    float v = 0;

    result = vec2(
        screenTex(gl_FragCoord.xy + vec2(-1, 0)).r,
      //screenTex(gl_FragCoord.xy + vec2( 0, 0)).r,
        screenTex(gl_FragCoord.xy + vec2( 1, 0)).r
    );
    
    v = dot(result, weights);
    FragColor = vec4(v, v, v, 1);
}