#version 330 core
in vec2 V_TexCoord;
uniform sampler2D U_Texture;
out vec4 outColor;
void main()
{
	vec4 color =vec4(0.0);
	int colorSize =3;
	float texelOffset =1/200;
	float kernel[9];
	kernel[0]=1; kernel[1]=2; kernel[2]=1;
	kernel[3]=2; kernel[4]=4; kernel[5]=2;
	kernel[6]=1; kernel[7]=2; kernel[8]=1;
	int index =0;
	for(int y=0; y<colorSize; y++)
	{
		for(int x=0; x<colorSize; x++)
		{
			vec4 currentColor =texture2D(U_Texture, V_TexCoord+vec2(float(-1+x)*texelOffset,float(-1+y)*texelOffset) );
			color +=currentColor*kernel[index++];
		}
	}
	color/=16.0;
	outColor =color;
}