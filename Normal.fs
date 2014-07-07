#pragma include TextureUtil.fsh

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
    FragColor = fetch(Texture, gl_FragCoord.xy);
}