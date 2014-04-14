#version 150

#pragma include TextureUtil.fsh
#pragma include Codec.fsh

in vec3 Position;
in vec2 TexCoord;
uniform sampler2D Texture;
flat out vec4 Color; // ScatterID
out vec2 FragTexCoord;

void main()
{
    ivec2 uv_coord = ivec2(Position.xy);
    ivec2 uv = uv_coord + ivec2(1, 0);
    float neighbor = fetch(Texture, uv).z;
    float curPixel = fetch(Texture, uv_coord).z;
    ivec2 root_xy = decode(curPixel);
    float scatterID = neighbor;
    Color = vec4(vec3(scatterID), 1);
    gl_Position = vec4(getScreenSpaceCoord(Texture, root_xy), scatterID, 1);
    FragTexCoord = TexCoord;
}