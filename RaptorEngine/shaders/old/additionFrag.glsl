#version 330
in vec2 outTextCord;
 uniform sampler2D tex1;
 uniform sampler2D tex2;
 out vec4 outputColor;
void main()
{
  vec4 color1=texture(tex1, outTextCord);
 vec4 color2=texture(tex2, outTextCord);
outputColor=color2-color1;
	
}
