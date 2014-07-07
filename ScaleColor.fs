#pragma include TextureUtil.fsh

uniform sampler2D Texture;
uniform float     Scale;

out vec4 FragColor;

void main()
{
    vec3 color = fetch(Texture, gl_FragCoord.xy).rgb;
    FragColor = vec4(color * Scale, 1);
}