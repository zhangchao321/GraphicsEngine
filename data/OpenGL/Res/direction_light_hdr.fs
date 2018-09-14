#version 330 core
uniform vec4 U_AmbientMaterial;
uniform vec4 U_DiffuseMaterial;
uniform vec4 U_SpecularMaterial;
uniform vec4 U_AmbientLight;
uniform vec4 U_DiffuseLight;
uniform vec4 U_SpecularLight;
uniform vec4 U_CameraPos;
uniform vec4 U_LightPos;
in vec4 V_Normal;
in vec4 V_WorldPos;

void main()
{
	vec3 L =U_LightPos.xyz;
	L =normalize(L);
	vec3 n =normalize(V_Normal.xyz);
	float diffuseIntensity =max(0.0,dot(L,n));
	vec4 diffuseColor =U_DiffuseMaterial*U_DiffuseLight*diffuseIntensity;
	vec4 specularColor =vec4(0.0);
	if(diffuseIntensity>0.0)
	{
		vec3 reflecDir =reflect(-L,n);
		reflecDir =normalize(reflecDir);
		vec3 worldPos =V_WorldPos.xyz;
		vec3 viewDir =normalize(U_CameraPos.xyz-worldPos);
		specularColor =U_SpecularLight*U_SpecularMaterial*pow(max(0,dot(reflecDir,viewDir)),4.0);
	}
	vec4 color =U_AmbientLight*U_AmbientMaterial+diffuseColor+specularColor;
	if(color.r>1.0)
	{
		gl_FragData[1] =color;
	}
	else
	{
		gl_FragData[0] =color;
	}
}