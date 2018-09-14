#version 330 core
in vec2 V_TexCoord;
uniform sampler2D U_Texture;
uniform sampler2D U_HDRTexture;
out vec4 outColor;
void main()
{
	outColor =texture2D(U_Texture,V_TexCoord)+texture2D(U_HDRTexture,V_TexCoord);
}