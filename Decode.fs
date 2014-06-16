#version 150

#pragma include TextureUtil.fsh
#pragma include Codec.fsh

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
    ivec2 current_xy = ivec2(gl_FragCoord.xy);
    float value      = fetch(Texture, current_xy).a;
    FragColor        = vec4(decode(value) / vec2( size(Texture) ), 0, value);
}