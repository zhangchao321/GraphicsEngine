#version 330 core
in vec2 V_TexCoord;
uniform sampler2D U_Texture1;
uniform sampler2D U_Texture2;
out vec4 outColor;
void main()
{
	vec4 baseColor=texture2D(U_Texture1,V_TexCoord);
	vec4 blendColor=texture2D(U_Texture2,V_TexCoord);
	outColor=vec4(vec3(1.0)-abs(vec3(1.0)-baseColor.rgb-blendColor.rgb),1.0);
}