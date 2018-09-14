#version 330 core
uniform sampler2D U_Texture1;
uniform sampler2D U_Texture2;
in vec2 V_TexCoord;
out vec4 outColor;
void main()
{
	vec4 baseColor =texture2D(U_Texture1,V_TexCoord);
	vec4 blendColor =texture2D(U_Texture2,V_TexCoord);
	outColor =vec4(baseColor.rgb-blendColor.rgb,1.0);
}