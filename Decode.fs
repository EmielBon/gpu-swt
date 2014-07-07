#pragma include TextureUtil.fsh
#pragma include Codec.fsh

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
    ivec2 current_xy = ivec2(gl_FragCoord.xy);
    float value      = fetch(Texture, current_xy).a;
    vec2  xy         = decode(value) / vec2( size(Texture) );
    FragColor        = vec4(xy, 0, value);
}