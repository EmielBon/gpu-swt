#version 150

#pragma include TextureUtil.fsh
#pragma include Codec.fsh

uniform sampler2D Texture;

flat in  float NeighborRootID;
     in  vec2  FragTexCoord;
     out vec4  FragColor;

void main()
{
    if (NeighborRootID == 0.0) // How is this even possible?
        discard;
    FragColor = vec4(decode(NeighborRootID) / vec2( textureSize(Texture, 0) ), NeighborRootID, 1);
}