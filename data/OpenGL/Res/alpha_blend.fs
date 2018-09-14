#version 330 core
uniform sampler2D U_Texture1;
uniform sampler2D U_Texture2;
in vec2 V_TexCoord;
out vec4 outColor;
void main()
{
	outColor =texture2D(U_Texture1,V_TexCoord)*0.5+texture2D(U_Texture2,V_TexCoord)*0.5;
}