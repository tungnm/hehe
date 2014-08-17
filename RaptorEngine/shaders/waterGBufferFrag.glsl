#version 330
 in vec3 onormal;
 in vec3 opos;
 //for cubemap reflection:
in vec3 reflectDir;
uniform samplerCube CubeMapTex; 

//layout (location = 0) out vec4 outputColor;
layout (location = 1) out vec3 posToG;
layout (location = 2) out vec3 normToG;
layout (location = 3) out vec4 diffuseToG;

void main()
{
posToG=opos;
normToG=onormal;
diffuseToG=vec4(texture(CubeMapTex,reflectDir).xyz,-100);
}