#version 330

//FOR LIGHTING CALCULATION
 in vec3 L;
 in vec3 V;
 in vec3 outNorm;

in vec3 reflectDir;
uniform samplerCube CubeMapTex; 
uniform int isSkybox;

 out vec4 outputColor;
 smooth in vec4 lightNDCPos;
uniform sampler2DShadow shadowMap;
	
void main()
{

	

	//vec4 light=vec4(1.0,0.0,8.0,1.0);
	//because in world space, so need to pass in camera position:
	//vec3 L=normalize(light.xyz-pos);//the light here is static in camera space, we could think like the viewer hold and
	//move the light with him during camera movement.
	//vec3 R=reflect(-L,norm);
	//vec4 rVtrans=inverseMV*vec4(rV,0.0);

		//shadow compute:
	 
	 //pcf:+
	 float fragmentshadow=1.0;
	vec3 cubeTexture=texture(CubeMapTex,reflectDir);
	outputColor=vec4(cubeTexture,1.0);
	 if(isSkybox==0)
	 {
	 float sum=0.0f;
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(-1,-1));
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(-1,1));
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(1,1));
	 sum+=textureProjOffset(shadowMap,lightNDCPos, ivec2(1,-1));
 
	 fragmentshadow=max(sum*0.25,0.2);
	//  *fragmentshadow;
	  vec3 l=normalize(L);
	vec3 v=normalize(V);
	vec3 n=normalize(outNorm);

		float kd=max(dot(l,n),0.2);
	
	vec3 R=reflect(-l,n);

	vec3 specular=vec3(0.0);
	
	vec3 diffuse=cubeTexture*0.33+vec3(0.1,0.3,0.7)*kd;//blueish color

	vec3 ambient=vec3(0.1,0.1,0.1);
	
	if (kd>0.0)
	specular=vec3(0.5,0.5,0.5)*pow(max(dot(R,v),0.0),14);

	  outputColor=vec4(ambient+(diffuse+specular)*fragmentshadow,1.0);
	 }

	
	
	
}
