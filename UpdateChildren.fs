#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
    ivec2 current_xy = ivec2(gl_FragCoord.xy);
    float current_id = fetch(Texture, current_xy).a;
    ivec2 root_xy    = decode(current_id);
    float root_id    = fetch(Texture, root_xy).a;
    FragColor        = vec4(0, 0, 0, root_id);
}