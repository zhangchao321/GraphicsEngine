#version 330 core
in vec2 V_TexCoord;
uniform sampler2D U_Texture1;
uniform sampler2D U_Texture2;
out vec4 outColor;
void main()
{
	vec4 baseColor=texture2D(U_Texture1,V_TexCoord);
	vec4 blendColor=texture2D(U_Texture2,V_TexCoord);
	outColor=abs(vec4(blendColor.rgb-baseColor.rgb,1.0));
}