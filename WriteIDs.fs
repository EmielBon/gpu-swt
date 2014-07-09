#pragma include Util.fsh
#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
    ivec2 current_xy = ivec2(gl_FragCoord.xy);
    vec4  bbox = fetch(Texture, current_xy);
    float current_id = ifelse(bbox.x == 0.0, 0.0, encode(current_xy));
    FragColor = vec4(current_id, 0, 0, 1);
}