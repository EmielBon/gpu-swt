#pragma include Util.fsh
#pragma include TextureUtil.fsh

uniform sampler2D Texture;
uniform vec2      StencilCenter;

     in  vec3 Position;
flat out vec4 Color;

void main()
{
    ivec2 current_xy = ivec2(Position.xy);
    Color            = fetch(Texture, current_xy);
    int   keep       = ifelse(Color.r == 0.0, -1, 1);
    vec2  pos        = getScreenSpaceCoord(Texture, StencilCenter * keep);
    gl_Position      = vec4(pos, 0, 1);
}