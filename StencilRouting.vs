#pragma include Util.fsh
#pragma include TextureUtil.fsh

uniform sampler2D BoundingBoxes;
uniform vec2      StencilCenter;

     in  vec3 Position;
flat out vec4 BoundingBox;

void main()
{
    ivec2 current_xy = ivec2(Position.xy);
    BoundingBox      = fetch(BoundingBoxes, current_xy);
    int   keep       = ifelse(BoundingBox.x == 0.0, -1, 1);
    vec2  pos        = getScreenSpaceCoord(BoundingBoxes, StencilCenter * keep);
    gl_Position      = vec4(pos, keep, ifelse(keep == 1, 1, 0));
}