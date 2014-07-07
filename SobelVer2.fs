uniform sampler2D Texture;

const vec2 weights = vec2(-1,/*0,*/1);

out vec4 FragColor;

vec4 fetch(sampler2D sampler, vec2 xy)
{
    return texelFetch(sampler, ivec2(xy), 0);
}

void main()
{
    vec2 result;
    float v = 0;
    
    result = vec2(
        fetch(Texture, gl_FragCoord.xy + vec2( 0,-1)).r,
      //fetch(Texture, gl_FragCoord.xy + vec2( 0, 0)).r,
        fetch(Texture, gl_FragCoord.xy + vec2( 0, 1)).r
    );
    
    v = dot(result, weights);
    FragColor = vec4(v, v, v, 1);
}