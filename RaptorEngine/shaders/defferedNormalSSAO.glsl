#version 330
//note that for a full screen quad, all computation need to be infragment shader because we need the interpolated value of TexCord to retrieve the data from G-Buffer
in vec2 outTextCord;
//textures handles from G-Buffer
uniform sampler2D gPosTex;
uniform sampler2D gNormTex;
uniform sampler2D gDiffuseTex;
uniform sampler2D gTanTex;
uniform sampler2D gNormalMapTex;
//SSAO random texture
uniform sampler2D rnm;
float radius=0.5;
float falloff=0.000002;
//for shadow mapping:----------------
uniform mat4 MVPLight;
//modelViewProj matrix to transform to light spect
uniform mat4 camProj;
//SSAO parametter
float g_scale=0.8f;
float g_intensity=0.71f;
float g_bias=-0.05f;
float g_sampleRad=0.25;
uniform int ssaoOn=1;
uniform vec3 lightPosViewSpace;
//shadow map texture from shadow FBO
uniform sampler2DShadow shadowMap;
out vec4 outputColor;

float doAO(vec2 tcoord, vec2 uv, vec3 p, vec3 cNorm)
 {
    vec3 diff=texture(gPosTex, tcoord+uv).xyz-p;
    vec3 v=normalize(diff);
    float d=length(diff)*g_scale;
    float result=max(0.0,dot(cNorm, v)-g_bias)*(1.0/(1.0+d))*g_intensity;
    return result;
}

void main()
{
    vec4 texColor=texture(gDiffuseTex, outTextCord);
   
    if (texColor.a<-199.0) //fire classified with the hardcoded value in normDepth.a
	{
        outputColor=vec4(texColor.xyz,1.0);
        return;
    }
	 vec4 normDepth=texture(gNormTex,outTextCord);
	//calculate L and V:
	vec3 opos=texture(gPosTex,outTextCord).xyz;
    //sample diffuse map:
	vec3 onormal=normDepth.xyz;
    vec3 n=onormal;
  
  // UNCOMMENT IF DONT USE SKYBOX
    if(length(onormal)==0) //dont calculation on black empty pixel
	{
        outputColor=vec4(0.0,0.0,0.0,1.0);
        return;
    }
	
	vec3 l,v;
/*   if (!(texColor.a<-99)&&!(onormal[0]==1.0&&onormal[1]==1.0&&onormal[2]==1.0)) //no normal map for water
	{
        vec3 oTangent=texture(gTanTex,outTextCord).xyz;
        //------------------------------------------------------------------------------normal map:
	vec3 bitangent=cross(onormal,oTangent);
        mat3 TBN=mat3(
	oTangent.x,bitangent.x, onormal.x,
	oTangent.y,bitangent.y, onormal.y,
	oTangent.z,bitangent.z, onormal.z);
        //----------
	 l=normalize(TBN*(lightPosViewSpace-opos));
        //note: for some unknow reason, if I normalize L and V here, then pass to fragment shader, then bugs will happen
	 v=normalize(TBN*(-opos));
        // if I just pass like this and normalize them in fragment shader then it works
	//-----------------
			
	//sample normal map to get the normal:
	
	vec3 perturbedNorm=texture(gNormalMapTex,outTextCord).xyz;
        //---------------------------------------------------------------------------End normal map

	 n=perturbedNorm;
    }
	*/
//	else
	//{
       l=normalize(lightPosViewSpace-opos);
       v=normalize(-opos);
//   }

	float kd=max(dot(l,n),0.0);
    vec3 R=reflect(-l,n);
    vec3 specular=vec3(0.0);
    vec3 diffuse=texColor.xyz*kd;
    vec3 ambient=vec3(0.16,0.16,0.16);

	specular=vec3(0.3,0.3,0.3)*pow(max(dot(R,v),0.0),11);
    //calcuate shadow:
	//screen space coord from light view?
	vec4 lightNDCPos=mat4(0.5,0.0,0.0,0.0,
					0.0,0.5,0.0,0.0,
					0.0,0.0,0.5,0.0,
					0.5,0.5,0.5,1.0)*MVPLight*vec4(opos,1.0);
    lightNDCPos.z+=0.0003;
    float fragmentshadow=max(textureProj(shadowMap,lightNDCPos),0.2);
    //sum*0.25;
	if (fragmentshadow <1.0)
	specular =vec3(0.0);
    //my AO code:=======================================================================================================================
	float ao=0.0f;
    if(ssaoOn==1)//&&!(normDepth.a<-99))
	{
        //**SSAO Calculation**//
  vec2 vec[8]=vec2[](vec2(1,0),vec2(-1,0),vec2(0,1),vec2(0,-1)  ,vec2(0.25,0.75),vec2(-0.25,0.75),vec2(0.75,0.25),vec2(-0.75,0.25));
        int iterations = 8;

		//TODO: REMOVE HARDCODE HERE
        vec2 rvec = texture(rnm, outTextCord * vec2(800.0/64.0,600.0/64.0)).xy * 2.0 - 1.0;
        float rad = g_sampleRad/opos.z;
        for (int j = 0; j < iterations; ++j)
{
            vec2 temp=reflect(vec[j],rvec);
            vec2 coord1 = temp*rad;
            vec2 coord2 = vec2(coord1.x*0.707 - coord1.y*0.707,
                          coord1.x*0.707 + coord1.y*0.707);
            ao += doAO(outTextCord,coord1*0.25, opos, onormal);
            ao += doAO(outTextCord,coord2*0.5, opos,onormal);
            ao += doAO(outTextCord,coord1*0.75, opos, onormal);
            ao += doAO(outTextCord,coord2, opos, onormal);
        }


ao/=16;
    }


//point light attenuation:
//float distancetoLight=length(lightPosViewSpace-opos);
//    float attenuation=(0.005*distancetoLight+0.01*distancetoLight*distancetoLight+0.05);
    vec3 finalRadiance=ambient*(1.0-ao*1.5)+(diffuse+specular)*fragmentshadow;
  //  finalRadiance=finalRadiance/attenuation;
    outputColor=vec4(finalRadiance,1.0);
}

