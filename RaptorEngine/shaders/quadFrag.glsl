#version 330
in vec2 outTextCord;
 uniform sampler2D tex1;//tex 1 here for quad shader is the texture that will be rendered
 out vec4 outputColor;
void main()
{
	vec4 texColor=texture(tex1, outTextCord);
	outputColor=texColor;//
	

}
