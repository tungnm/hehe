#version 330
//matrix in view space
uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 normalTransform; 

//per vertex data
layout(location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
//layout (location=2) in vec2 texCord;

 //per vertex data, after transformed, to be write to the G-Buffer
 out vec3 onormal;
 out vec3 opos;
 //for water wave
  float PI=3.14159;
  uniform float time;

  //for cubemap reflection:
uniform mat4 M;
uniform mat3 normalTransformToWorld; 
uniform vec3 camPos;
out vec3 reflectDir;
void main()
{

//transform:

vec3 transformPos=position;
vec3 transformNorm=normal;

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


// END OF WAVE===============================================

//update positions:
transformPos.y=y;
transformPos.x=x1.x;
transformPos.z=x1.y;

transformNorm=normalize(transformNorm);

onormal=normalize(normalTransform*transformNorm);
opos=(MV*vec4(transformPos,1.0)).xyz;
//otexCord=texCord*2;
gl_Position=MVP*vec4(transformPos,1.0);

//calculate reflection vector for cubemap:
	vec3 worldPos=vec3(M*vec4(transformPos,1.0));
	vec3 worldNorm=normalize(normalTransformToWorld*transformNorm); 
	vec3 worldView=normalize(camPos-worldPos);
	reflectDir=reflect(-worldView,worldNorm);
	
}