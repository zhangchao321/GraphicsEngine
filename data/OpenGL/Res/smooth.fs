#version 330 core
in vec2 V_TexCoord;
uniform sampler2D U_Texture;
out vec4 outColor;
void main()
{
	vec4 color=vec4(0.0);
	int coreSize=3;
	float texelOffset=1/300.0;
	float kernel[9];
	kernel[6]=1;kernel[7]=1;kernel[8]=1;
	kernel[3]=1;kernel[4]=1;kernel[5]=1;
	kernel[0]=1;kernel[1]=1;kernel[2]=1;
	int index=0;
	for(int y=0;y<coreSize;y++)
	{
		for(int x=0;x<coreSize;x++)
		{
			vec4 currentColor=texture2D(U_Texture,V_TexCoord+vec2((-1+x)*texelOffset,(-1+y)*texelOffset));
			color+=currentColor*kernel[index++];
		}
	}
	color/=9.0;
	outColor=color;
}