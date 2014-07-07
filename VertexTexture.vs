#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;

in vec3 Position;

void main()
{
    vec2 pos     = getScreenSpaceCoord(Texture, Position.xy);
    gl_Position  = vec4(pos, 0, 1);
}