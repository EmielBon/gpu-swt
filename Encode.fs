#version 150

#pragma include TextureUtil.fsh
#pragma include Codec.fsh

uniform vec3      BackgroundColor = vec3(0, 0, 0);
uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
    ivec2 current_xy = ivec2(gl_FragCoord.xy);
    vec3  color      = fetch(Texture, current_xy).rgb;
    bool  foreground = (color != BackgroundColor);
    float value      = foreground ? encode(current_xy) : 0.0;
    FragColor        = vec4(0, 0, 0, value);
}