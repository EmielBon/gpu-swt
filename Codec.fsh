float encode(ivec2 position)
{
    int ret = (position.x + 1) | ((position.y + 1) << 11);
    return float(ret);
}

float encode(vec2 position)
{
    return encode(ivec2(position));
}

ivec2 decode(float value)
{
    int i = int(value);
    int x = (i & 0x000007FF) - 1;
    int y = ((i & 0x003FF800) >> 11) - 1;
    return ivec2(x, y);
}