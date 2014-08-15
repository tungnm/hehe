#version 330
 //in vec3 onormal;
 //in vec3 opos;
 //in vec3 oTangent;
 //in vec2 otexCord;

layout (location = 0) out vec4 outputColor;
//layout (location = 1) out vec3 posToG;
//layout (location = 2) out vec3 normToG;
layout (location = 3) out vec4 diffuseToG;
//layout (location = 4) out vec3 tanToG;
//layout (location = 5) out vec3 normalMapToG;

 //uniform sampler2D tex1;
 //uniform sampler2D normalMap;

void main()
{

//posToG=opos;
//normToG=onormal;
diffuseToG= vec4(0.92,0.92,0.92,-200.0);
//tanToG=oTangent;
//normalMapToG=normalize(texture(normalMap,otexCord).xyz*2.0-1.0);
outputColor=vec4(1.0);

}