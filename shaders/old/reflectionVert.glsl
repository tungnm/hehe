#version 330
//for lighting calculation in view space:
uniform mat4 MV;
uniform mat3 normalTransformView;
uniform vec3 lightPosViewSpace;
 out vec3 L;
 out vec3 V;
 out vec3 outNorm;

//--------------------------------------
uniform mat4 M;
uniform mat4 MVP;
uniform mat3 normalTransform; 
uniform vec3 camPos; //NOTE that for reflection we calculate reflection, refraction vector in vertex shader.
//the resulted then interpolated in fragment shader.

//note:
 //The fragment shader has access to the same set of uniforms as the vertex shader, 
 //but cannot declare or access attribute variables

layout(location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
uniform float time;
uniform float textureScale;
uniform bool isWater;
// smooth out vec3 pos;
 //smooth out vec3 norm;
 out vec3 reflectDir;
 uniform mat4 MVPLight;

 smooth out vec4 lightNDCPos;
 float PI=3.14159;
 //float paramX=0.03f;
 //float paramY=0.05f;
 //float sizeX=5;
 //float sizeY=3;
void main()
{
vec3 transformPos=position;
vec3 transformNorm=normal;
if(isWater)
{
//simple wave function
//
//float angle=mod(time/300,2*PI);
//transformPos.y=paramX*transformPos.x*sin(transformPos.z+angle)*sizeX+paramY*transformPos.z*cos(transformPos.x+angle)*sizeY;
////recalculate normal:
////df/dx
//transformNorm.x=-paramX*sin(transformPos.z+angle)*sizeX+paramY*transformPos.z*sin(transformPos.x+angle)*sizeY;
//transformNorm.y=1;
//transformNorm.z=-paramX*transformPos.x*cos(transformPos.z+angle)*sizeX-paramY*cos(transformPos.x+angle)*sizeY;
//transformNorm=normalize(transformNorm);
//
float time1=time/450;
////Gesner first wave:


float alpha=6.5*PI;
float A=3.5;
float w=1.754;

vec2 unitK=vec2(1.0,0.0);
float kMagnitude=2*PI/alpha;//==1/3
vec2 k=unitK*kMagnitude;
vec2 x0=vec2(transformPos.x,transformPos.z);

//transformed x,z:

vec2 x1=x0-unitK*A*sin(dot(k,x0)-w*time1);
float y=A*cos(dot(k,x0)-w*time1);
//y=A*cos(k.x*x.x+k.y*x.y-w*time1)
//y=A*cos(k.x*x+k.y*z-w*time1)

/*transformPos.y=y;
transformPos.x=x1.x;
transformPos.z=x1.y;
*/

//recalculate normal:
//df/dx
transformNorm.x=A*(sin(k.x*x0.x+k.y*x0.y-w*time1))*(k.x);
transformNorm.y=1;
//df/dz
transformNorm.z=A*(sin(k.x*x0.x+k.y*x0.y-w*time1))*(k.y);

//second wave:

alpha=8*PI;
A=2.8;
w=1.581;//sqrt(4)

unitK=vec2(0.707,0.707);
kMagnitude=2*PI/alpha;//==2/5
k=unitK*kMagnitude;
//vec2 x0=vec2(transformPos.x,transformPos.z);


//transformed x,z:

x1-=/*x0-*/unitK*A*sin(dot(k,x0)-w*time1+10);
y+=A*cos(dot(k,x0)-w*time1+10);
//y=A*cos(k.x*x.x+k.y*x.y-w*time1)
//y=A*cos(k.x*x+k.y*z-w*time1)

//recalculate normal:
//df/dx
transformNorm.x+=A*(sin(k.x*x0.x+k.y*x0.y-w*time1))*(k.x);
transformNorm.y=1;
//df/dz
transformNorm.z+=A*(sin(k.x*x0.x+k.y*x0.y-w*time1))*(k.y);


//////third wave
//
//alpha=45*PI;
//A=12;
//w=0.666;//sqrt(4)
//
//unitK=vec2(-0.707,0.707);//0.8944,0.4472);
//kMagnitude=2*PI/alpha;//==2/5
//k=unitK*kMagnitude;
////vec2 x0=vec2(transformPos.x,transformPos.z);
//
//
////transformed x,z:
//
//x1-=/*x0-*/unitK*A*sin(dot(k,x0)-w*time1+30);
//y+=A*cos(dot(k,x0)-w*time1+30);
////y=A*cos(k.x*x.x+k.y*x.y-w*time1)
////y=A*cos(k.x*x+k.y*z-w*time1)
//
////recalculate normal:
////df/dx
//transformNorm.x+=A*(sin(k.x*x0.x+k.y*x0.y-w*time1))*(k.x);
//transformNorm.y=1;
////df/dz
//transformNorm.z+=A*(sin(k.x*x0.x+k.y*x0.y-w*time1))*(k.y);


// END OF WAVE===============================================

//update positions:
transformPos.y=y;
transformPos.x=x1.x;
transformPos.z=x1.y;



transformNorm=normalize(transformNorm);

}
	//it does not matter to use normal/light vector in world space or view space for normal Phong model
	//because the relative position of these
	//two vector are the same and their dot product will be the same 
	//---calculate vectors need for reflection in world space:
	vec3 worldPos=vec3(M*vec4(transformPos,1.0));
	vec3 worldNorm=normalize(normalTransform*transformNorm); 
	vec3 worldView=normalize(camPos-worldPos);
	reflectDir=reflect(-worldView,worldNorm);//refract(-worldView, worldNorm, 0.67 );//
	//RefractDir = refract(-worldView, worldNorm, 
      //                       0.8 );
	  //calcuate shadow:
	lightNDCPos=mat4(0.5,0.0,0.0,0.0,
					0.0,0.5,0.0,0.0,
					0.0,0.0,0.5,0.0,
					0.5,0.5,0.5,1.0)*MVPLight*vec4(transformPos,1.0);
	 //lighting calculation
	 vec3 opos=(MV*vec4(transformPos,1.0)).xyz;
	 L=(lightPosViewSpace-opos);//note: for some unknow reason, if I normalize L and V here, then pass to fragment shader, then bugs will happen
	 V=(-opos);// if I just pass like this and normalize them in fragment shader then it works
	outNorm=normalize(normalTransformView*transformNorm);

	gl_Position = MVP*vec4(transformPos.x,transformPos.y,transformPos.z,1.0);//
	
}