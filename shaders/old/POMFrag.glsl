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
 uniform float shadowWeight=0.0f;

 out vec4 outputColor;

void main()
{




	vec3 l=normalize(L);
	vec3 v=normalize(V);

	
	//parallax occlusion map: modify the texture coordinate
	float nSteps=mix(150, 20, v.z);

	float steP=1.0/nSteps;
	vec2 delta=-v.xy*0.05/(v.z*nSteps);
	float currentHeight=1.0;//start at the top
	vec2 finalTexCord=oTexCord; //initial texCord
	float sampledHeight= texture(heightMap, oTexCord).x;
	int count=0;

	//steping through the heightmap with direction delta:
	while(sampledHeight<currentHeight && count<nSteps)
	{
		currentHeight-=steP;
		finalTexCord+=delta;
		count++;
		sampledHeight= texture(heightMap, finalTexCord).x;
	}
	
	
	//-------------End of parallax occlusion map, remove this to remove parallax

	//sample normal map to get the normal:
	vec3 perturbedNorm=normalize(texture(normalMap, finalTexCord).xyz*2.0-1.0);//
	vec3 n=perturbedNorm;

	float kd=max(dot(l,n),0.0);
	vec3 shadow=vec3(0.0,0.0,0.0);
	if(kd>0.0)
	{
	//shadow tracing:
	vec2 shadowCord=finalTexCord; //set starting cord to trace shadow ray

	float nShadowSteps=mix(60,20,l.z);
	steP=1.0/nShadowSteps;
	delta=l.xy*0.05/(l.z*nShadowSteps);
	currentHeight=	sampledHeight+steP*0.01; //sampledHeight still retained the height at the point we found
	while(sampledHeight<currentHeight && currentHeight<1.0) //trace, follow light direction, step up
	{//keep going if the sampled height < current height on ray( so not block yet, keep going), or go out of surface
		currentHeight+=steP;
		shadowCord+=delta;
		sampledHeight= texture(heightMap, shadowCord).x;
	}
	//after the loop if currentHeight< sampledheight-> in shadow
	
	if (currentHeight<sampledHeight)
		shadow=vec3(-0.1,-0.1,-0.1);

	}

	vec3 R=reflect(-l,n);

	vec3 specular=vec3(0.0);
	
	//sample diffuse map:
	vec4 texColor=texture(tex1, finalTexCord);
	vec3 diffuse=texColor.xyz*kd;

	vec3 ambient=vec3(0.1,0.1,0.1);
	if (kd>0.0)
	specular=vec3(0.5,0.5,0.5)*pow(max(dot(R,v),0.0),9);

		//shadow compute:
	 //pcf:
	 float sum=0.0f;
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(-1,-1));
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(-1,1));
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(1,1));
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(1,-1));
 
	 float fragmentshadow=sum*0.25;


	outputColor=vec4(ambient+(diffuse+specular)*fragmentshadow+mix(shadow,vec3(0.0),shadowWeight),1.0);
}
