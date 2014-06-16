#version 150

#pragma include Codec.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;

in  vec3  Position;
in  vec2  TexCoord;
out vec2  FragTexCoord;

flat out ivec2 PixelPosition;

void main()
{
    // todo: remove
    FragTexCoord  = TexCoord;
    
    PixelPosition = ivec2(Position.xy);
    vec2 pos = getScreenSpaceCoord(Texture, ivec2(1, 1));
    gl_Position = vec4(pos, 0, 1);
}