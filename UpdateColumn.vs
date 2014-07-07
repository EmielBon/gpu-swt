#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform int Column;
uniform sampler2D Texture;

in vec3 Position;

void main()
{
    ivec2 current_xy = ivec2(Position.xy) + ivec2(Column, 0);
    gl_Position = vec4(getScreenSpaceCoord(Texture, current_xy), 0, 1);
}