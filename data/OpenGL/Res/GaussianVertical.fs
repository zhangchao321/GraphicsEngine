#version 330 core
in vec2 V_TexCoord;
uniform sampler2D U_Texture;
out vec4 outColor;
void main()
{
	float texelOffset =1/200;
	float weight[5] =float[](0.22,0.19,0.12,0.08,0.01);
	vec4 color =texture2D(U_Texture,V_TexCoord)*weight[0];
	for(int i=1; i<5; i++)
	{
		color +=texture2D(U_Texture,vec2(V_TexCoord.x,V_TexCoord.y+texelOffset*float(i)))*weight[i];
		color +=texture2D(U_Texture,vec2(V_TexCoord.x,V_TexCoord.y-texelOffset*float(i)))*weight[i];
	}
	outColor =color;
}