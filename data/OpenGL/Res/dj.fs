#version 330 core
uniform sampler2D U_Texture1;
uniform sampler2D U_Texture2;
in vec2 V_TexCoord;
out vec4 outColor;
void main()
{
	vec4 lumCoeff=vec4(0.2125,0.7154,0.0721,1.0);
	vec4 baseColor =texture2D(U_Texture1,V_TexCoord);
	vec4 blendColor =texture2D(U_Texture2,V_TexCoord);
	
	float luminance=dot(baseColor.rgb,lumCoeff.rgb);
	if(luminance<0.45)
	{
		outColor=2.0*blendColor*baseColor;
	}
	else if(luminance>0.55)
	{
		outColor=vec4(1.0)-2.0*(vec4(1.0)-baseColor)*(vec4(1.0)-blendColor);
	}
	else
	{
		vec4 color1=2.0*blendColor*baseColor;
		vec4 color2=vec4(1.0)-2.0*(vec4(1.0)-baseColor)*(vec4(1.0)-blendColor);
		outColor=mix(color1,color2,(luminance-0.45)*10.0);
	} 
}