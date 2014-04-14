#version 150

#pragma include TextureUtil.fsh
#pragma include Codec.fsh

uniform vec3      BackgroundColor = vec3(0, 0, 0);
uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
    vec3 color = fetch(Texture, gl_FragCoord.xy).rgb;
    bool foreground = (color != BackgroundColor);
    float value = int(foreground) * encode(gl_FragCoord.xy);
    FragColor = vec4(decode(value) / vec2(textureSize(Texture, 0)), value, 1);
}