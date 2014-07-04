ivec2 size(sampler2D sampler)
{
    return textureSize(sampler, 0);
}

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
    return texture(sampler, xy / size(sampler));
}

vec2 getScreenSpaceCoord(sampler2D sampler, vec2 screenCoord)
{
    // todo: no idea why the +0.5 only with x and not with y
    vec2 pos = screenCoord + vec2(0.5, 0);
    return (pos / (size(sampler) - vec2(0.5))) * 2 - vec2(1);
    //return ((screenTexCoord + vec2(0.5, 0))) / (size(sampler) - vec2(0.5)) * 2 - vec2(1);
}

vec2 getScreenSpaceTexCoord(sampler2D sampler, vec2 screenCoord)
{
    return (getScreenSpaceCoord(sampler, screenCoord) + 1) / 2;
}