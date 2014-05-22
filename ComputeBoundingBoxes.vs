#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;
uniform vec2      ValueMask;

flat out float Value;

void main()
{
    ivec2 current_xy = ivec2(Position.xy);
    float current_root_id = fetch(Texture, current_xy).a;
    vec2  current_root_xy = decode(current_root_id);
    vec2  pos = getScreenSpaceCoord(Texture, current_root_xy);
    Value = dot(current_xy, ValueMask);
    gl_Position = vec4(pos, Value, 1);
}