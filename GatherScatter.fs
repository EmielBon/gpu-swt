#version 150

#pragma include TextureUtil.fsh
#pragma include Codec.fsh

uniform sampler2D Texture;

flat in  float ScatterID;
     out vec4  FragColor;

void main()
{
    if (ScatterID == 0.0) // todo: eliminate this check by making it impossible that this occors
        discard;
    FragColor = vec4(0, 0, 0, ScatterID);
}