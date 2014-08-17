#version 330

uniform mat4 MVP;
uniform mat4 M;

in vec3 position;


uniform float textureYOffset;
uniform float textureXOffset;

smooth out vec2 outTexCoord;
out vec2 fragmentScreenSpacePos;

void main()
{
	vec4 clipSpace= MVP*vec4(position.x,position.y,position.z,1.0);
	//vec4 fragWorld=M*vec4(position.x,position.y,position.z,1.0);

	gl_Position =clipSpace;// + totalOffset;
	
	//vec3 NDCSpace = clipSpace.xyz / clipSpace.w;
	vec3 screenSpace = 0.5 *  clipSpace.xyz / clipSpace.w + 0.5;
	outTexCoord=screenSpace.xy+vec2(textureXOffset ,-textureYOffset);

	fragmentScreenSpacePos=screenSpace.xy;
}