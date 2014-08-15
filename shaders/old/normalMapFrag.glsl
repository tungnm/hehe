#version 330
  in vec2 oTexCord;

 in vec3 L;
 in vec3 V;

 uniform sampler2D tex1;
 uniform sampler2D normalMap;

 uniform float lightStrength;//from the fire

  in float attenuation;
 //light space coordinate, used for shadow map
smooth in vec4 lightNDCPos;
uniform sampler2DShadow shadowMap;

//===========================================

 //controler for parallax mapping:
 uniform float shadowWeight=0.0f;

 out vec4 outputColor;

void main()
{

	vec3 l=normalize(L);
	vec3 v=normalize(V);
	vec2 finalTexCord=oTexCord; //initial texCord
	


	//sample normal map to get the normal:
	vec3 perturbedNorm=normalize(texture(normalMap, finalTexCord).xyz*2.0-1.0);//
	vec3 n=perturbedNorm;

	float kd=max(dot(l,n),0.2);
	
	vec3 R=reflect(-l,n);

	vec3 specular=vec3(0.0);
	
	//sample diffuse map:
	vec4 texColor=texture(tex1, finalTexCord);
	vec3 diffuse=texColor.xyz*kd;

	vec3 ambient=vec3(0.1,0.1,0.1);
	if (kd>0.0)
	specular=vec3(0.4,0.4,0.4)*pow(max(dot(R,v),0.0),9);
	//specular*=lightStrength;
		//shadow compute:
	 //pcf:
	 float sum=0.0f;
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(-1,-1));
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(-1,1));
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(1,1));
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(1,-1));
 
	 float fragmentshadow=max(sum*0.25,0.2);


	outputColor=vec4((ambient+(diffuse+specular)*fragmentshadow)*lightStrength/attenuation,1.0);
}
