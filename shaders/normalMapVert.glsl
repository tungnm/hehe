#version 330
//matrix in view space
uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 normalTransform; 

//per vertex data
layout(location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location=2) in vec2 texCord;
//layout (location=3) in vec4 tangent;

 //per vertex data, after transformed, to be write to the G-Buffer
 out vec3 onormal;
 out vec3 opos;
// out vec3 oTangent;
 out vec2 otexCord;

void main()
{

//transform:

vec3 transformPos=position;
vec3 transformNorm=normal;

onormal=normalize(normalTransform*transformNorm);
//oTangent=normalize(normalTransform*tangent.xyz);
opos=(MV*vec4(transformPos,1.0)).xyz;
otexCord=texCord;

gl_Position=MVP*vec4(transformPos,1.0);

	
}