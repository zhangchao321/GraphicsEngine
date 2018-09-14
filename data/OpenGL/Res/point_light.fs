#version 330 core 
uniform vec4 U_AmbientMaterial;
uniform vec4 U_DiffuseMaterial;
uniform vec4 U_SpecularMaterial;
uniform vec4 U_AmbientLight;
uniform vec4 U_DiffuseLight;
uniform vec4 U_LightPos;
uniform vec4 U_SpecularLight;
uniform vec4 U_CameraPos;
in vec4 V_Position;
in vec4 V_Normal;
out vec4 outColor;
void main()
{
	
	vec3 L =U_LightPos.xyz-V_Position.xyz;
	
	float constantFactor =0.5;
	float linearFactor =0.3;
	float expFactor =0.1;
	float distance =length(L);
	float attenuation =1/(constantFactor+linearFactor*distance+expFactor*distance*distance);
	
	L =normalize(L);
	vec3 n=normalize(V_Normal.xyz);
	float diffuseIntensity =max(0.0,dot(L,n));
	vec4 diffuseColor =U_DiffuseLight*U_DiffuseMaterial*diffuseIntensity*attenuation;
	
	vec4 specularColor=vec4(0.0,0.0,0.0,0.0);
	if(diffuseIntensity>0.0)
	{
		vec3 reflectVec =reflect(-L,n);
		reflectVec =normalize(reflectVec);
		vec3 worldPos =V_Position.xyz;
		vec3 viewVec =U_CameraPos.xyz-worldPos;
		viewVec =normalize(viewVec);
		specularColor =U_SpecularLight*U_SpecularMaterial*pow(max(0,dot(viewVec,reflectVec)),4.0);
	} 
	outColor =diffuseColor+U_AmbientLight*U_AmbientMaterial+specularColor;
}