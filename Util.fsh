ivec2 swizzleIf(bool condition, ivec2 v)
{
    return ivec2(mix(v.xy, v.yx, int(condition)));
}

float ifelse(bool condition, float a, float b)
{
    return mix(a, b, int(!condition));
}

int ifelse(bool condition, int a, int b)
{
    return int(ifelse(condition, float(a), float(b)));
}

ivec2 ifelse(bool condition, ivec2 a, ivec2 b)
{
    return ivec2(mix(a, b, int(!condition)));
}

vec4 ifelse(bool condition, vec4 a, vec4 b)
{
    return mix(a, b, int(!condition));
}