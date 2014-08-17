#version 330
  in vec2 oTexCord;

 in vec3 L;
 in vec3 V;

 uniform sampler2D tex1;
 uniform sampler2D normalMap;
 uniform sampler2D heightMap;

 //light space coordinate, used for shadow map
smooth in vec4 lightNDCPos;
uniform sampler2DShadow shadowMap;
//===========================================


 //controler for parallax mapping:
 uniform float parallaxWeight=0.0f;

 out vec4 outputColor;

void main()
{


	vec2 finalTexCord=oTexCord;


	vec3 l=normalize(L);
	vec3 v=normalize(V);

	
	
	//parallax map: modify the texture coordinate
	float height= texture(heightMap, oTexCord).x;
	float newHeight=height*0.04-0.03;//note: the higher pixel will shift less than the lower: height=1 -> almost not shifted at all, height=0-> shift alot, opposite direction of the view vector
	vec2 newTexCord=oTexCord+(v.xy*newHeight);
	
	 finalTexCord=mix(oTexCord, newTexCord,parallaxWeight);
	//-------------End of parallax map, remove this to remove parallax

	//sample normal map to get the normal:
	vec3 perturbedNorm=normalize(texture(normalMap, finalTexCord).xyz*2.0-1.0);//
	vec3 n=perturbedNorm;


	vec3 R=reflect(-l,n);

	vec3 specular=vec3(0.0);
	float kd=max(dot(l,n),0.0);
	//sample diffuse map:
	vec4 texColor=texture(tex1, finalTexCord);
	vec3 diffuse=texColor.xyz*kd;

	vec3 ambient=vec3(0.1,0.1,0.1);
	if (kd>0.0)
	specular=vec3(0.5,0.5,0.5)*pow(max(dot(R,v),0.0),9);

	//shadow compute:
	 //pcf:
	 float sum=0;
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(-1,-1));
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(-1,1));
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(1,1));
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(1,-1));
 
	 float shadow=sum*0.25;


	outputColor=vec4(ambient+diffuse*shadow+specular,1.0);
}
