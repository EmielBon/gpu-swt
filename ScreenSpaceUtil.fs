#version 150

vec4 fetch(sampler2D sampler, vec2 xy)
{
    return texelFetch(sampler, ivec2(xy), 0);
}

vec4 sample(sampler2D sampler, vec2 xy)
{
    return texture(sampler, xy / textureSize(sampler, 0));
}