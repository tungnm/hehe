  #version 330
  //note that for a full screen quad, all computation need to be infragment shader because we need the interpolated value of TexCord to retrieve the data from G-Buffer
 in vec2 outTextCord;

 uniform sampler2D gPosTex;
 uniform sampler2D gNormTex;
 uniform sampler2D gDiffuseTex;
 uniform sampler2D gTanTex;
 uniform sampler2D gNormalMapTex;
 
//3 samplers to read from g buffer

 
//	in vec2 oTexCord;

 //in vec3 L;
 //in vec3 V;

 //from vertex shader:
 //for shadow mapping:----------------
uniform mat4 MVPLight;
//uniform vec3 directionalLight;
uniform vec3 lightPosViewSpace;
//smooth out vec4 lightNDCPos; //position in light space for comparing to shadow map

 //light space coordinate, used for shadow map
//smooth in vec4 lightNDCPos;
uniform sampler2DShadow shadowMap;
//===========================================

 //controler for parallax mapping:
 uniform float shadowWeight=0.0f;
 out vec4 outputColor;


void main()
{
	//calculate L and V:
	vec4 normDepth=texture(gNormTex,outTextCord);
	vec3 onormal=normDepth.xyz;
	vec3 oTangent=texture(gTanTex,outTextCord).xyz;
	vec3 opos=texture(gPosTex,outTextCord).xyz;

//	oTexCord=texture(gTexTex,outTextCord).xyz; //2 different textures, one is the texture of the quad, the other one is the texture coordinate store in another texture
	
	//normal map:
	vec3 bitangent=cross(onormal,oTangent);
	mat3 TBN=mat3(
	oTangent.x,bitangent.x, onormal.x,
	oTangent.y,bitangent.y, onormal.y,
	oTangent.z,bitangent.z, onormal.z);

	//----------
	vec3 L=TBN*(lightPosViewSpace-opos);//note: for some unknow reason, if I normalize L and V here, then pass to fragment shader, then bugs will happen
	 vec3 V=TBN*(-opos);// if I just pass like this and normalize them in fragment shader then it works
	
	//calcuate shadow:
	vec4 lightNDCPos=mat4(0.5,0.0,0.0,0.0,
					0.0,0.5,0.0,0.0,
					0.0,0.0,0.5,0.0,
					0.5,0.5,0.5,1.0)*MVPLight*vec4(opos,1.0);
	





	//-----------------







	vec3 l=normalize(L);
	vec3 v=normalize(V);
	//vec2 finalTexCord=oTexCord; //initial texCord
	


	//sample normal map to get the normal:
	vec3 perturbedNorm=texture(gNormalMapTex,outTextCord).xyz;//normalize(texture(normalMap, finalTexCord).xyz*2.0-1.0);//
	vec3 n=perturbedNorm;

	float kd=max(dot(l,n),0.2);
	
	vec3 R=reflect(-l,n);

	vec3 specular=vec3(0.0);
	
	//sample diffuse map:
	vec4 texColor=texture(gDiffuseTex, outTextCord);
	vec3 diffuse=texColor.xyz*kd;

	vec3 ambient=vec3(0.1,0.1,0.1);
	if (kd>0.2)
	specular=vec3(0.5,0.5,0.5)*pow(max(dot(R,v),0.0),9);
	
	//lightNDCPos.z+=0.0019;
		//shadow compute:
	 //pcf:
	// float sum=0.0f;
	// sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(-1,-1));
	// sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(-1,1));
	// sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(1,1));
	// sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(1,-1));
 
 //float fragmentshadow=max(sum*0.25,0.5);
	float fragmentshadow=max(textureProj(shadowMap,lightNDCPos),0.2);//sum*0.25;


//	outputColor=vec4(ambient+(diffuse+specular)*fragmentshadow,1.0);
	outputColor=vec4(normDepth.a,normDepth.a,normDepth.a,1.0);
}
