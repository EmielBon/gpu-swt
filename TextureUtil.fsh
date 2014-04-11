vec4 fetch(sampler2D sampler, ivec2 xy)
{
    return texelFetch(sampler, xy, 0);
}

vec4 fetch(sampler2D sampler, vec2 xy)
{
    return fetch(sampler, ivec2(xy));
}

vec4 sample(sampler2D sampler, vec2 xy)
{
    return texture(sampler, xy / textureSize(sampler, 0));
}

vec2 getScreenSpaceCoord(sampler2D sampler, vec2 screenTexCoord)
{
    return screenTexCoord / (textureSize(sampler, 0) - vec2(1)) * 2 - vec2(1);
}