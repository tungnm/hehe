#version 330
//matrix in view space
uniform mat4 V;
uniform mat4 P;
//uniform mat3 normalTransform; 

//per vertex data
layout(location = 0) in vec3 position;
//layout (location = 1) in vec3 normal;
//layout (location=2) in vec2 texCord;
//layout (location=3) in vec4 tangent;
layout (location=4) in vec2 InstancePos;

 //per vertex data, after transformed, to be write to the G-Buffer
 //out vec3 onormal;
 //out vec3 opos;
 //out vec3 oTangent;
 //out vec2 otexCord;

void main()
{
//-i*cellHeight-cellHeight, 0.5, j*cellWidth+cellWidth);


mat4 model=mat4(0.35,0.0,0.0,0.0,
				0.0,0.35,0.0,0.0,
				0.0,0.0,0.35,0.0,
				-InstancePos[0]-1.0,0.0,InstancePos[1]+1.0,1.0);
//transform:

vec3 transformPos=position;
//vec3 transformNorm=normal;

//onormal=normalize(transformNorm);
//oTangent=normalize(tangent.xyz);
mat4 MV=V*model;
//opos=(MV*vec4(transformPos,1.0)).xyz;
//otexCord=texCord;

gl_Position=P*MV*vec4(transformPos,1.0);

	
}