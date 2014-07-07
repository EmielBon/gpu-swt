#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
    ivec2 current_xy      = ivec2(gl_FragCoord.xy);
    float current_root_id = fetch(Texture, current_xy).a;
    ivec2 current_root_xy = decode(current_root_id);
    FragColor             = fetch(Texture, current_root_xy);
}