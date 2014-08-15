#version 330

uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 normalTransform; 

//uniform vec3 directionalLight;

layout(location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location=2) in vec2 texCord;
layout (location=3) in vec4 tangent;

//for shadow mapping:----------------
uniform mat4 MVPLight;
//uniform vec3 directionalLight;
uniform vec3 lightPosViewSpace;
smooth out vec4 lightNDCPos; //position in light space for comparing to shadow map
//-----------------------------------
 out vec2 oTexCord;
 out vec3 L;
 out vec3 V;

 out float attenuation;

 float PI=3.14159;
 float paramX=0.03f;
 float paramY=0.05f;
 float sizeX=5;
 float sizeY=3;
 uniform float time;
uniform float textureScale;
uniform bool isWater;
void main()
{
vec3 transformPos=position;
vec3 transformNorm=normal;
if(isWater)
{
//simple wave function

float angle=mod(time/300,2*PI);
transformPos.y+=paramX*transformPos.x*sin(transformPos.z+angle)*sizeX+paramY*transformPos.z*cos(transformPos.x+angle)*sizeY;
//recalculate normal:
//df/dx
transformNorm.x=-paramX*sin(transformPos.z+angle)*sizeX+paramY*transformPos.z*sin(transformPos.x+angle)*sizeY;
transformNorm.y=1;
transformNorm.z=-paramX*transformPos.x*cos(transformPos.z+angle)*sizeX-paramY*cos(transformPos.x+angle)*sizeY;
transformNorm=normalize(transformNorm);
}




vec3 onormal=normalize(normalTransform*transformNorm);
vec3 oTangent=normalize(normalTransform*tangent.xyz);
vec3 opos=(MV*vec4(transformPos,1.0)).xyz;
oTexCord=texCord;
gl_Position=MVP*vec4(transformPos,1.0);

	//transformNorm map:
	vec3 bitangent=cross(onormal,oTangent);
	mat3 TBN=mat3(
	oTangent.x,bitangent.x, onormal.x,
	oTangent.y,bitangent.y, onormal.y,
	oTangent.z,bitangent.z, onormal.z);

	//----------
	 L=TBN*(lightPosViewSpace-opos);//note: for some unknow reason, if I normalize L and V here, then pass to fragment shader, then bugs will happen
	 V=TBN*(-opos);// if I just pass like this and normalize them in fragment shader then it works
	
	//calcuate shadow:
	lightNDCPos=mat4(0.5,0.0,0.0,0.0,
					0.0,0.5,0.0,0.0,
					0.0,0.0,0.5,0.0,
					0.5,0.5,0.5,1.0)*MVPLight*vec4(transformPos,1.0);
	
	
	 //point light attenuation:
float distancetoLight=length(lightPosViewSpace-opos);
attenuation=(0.09*distancetoLight+0.04*distancetoLight*distancetoLight);


}