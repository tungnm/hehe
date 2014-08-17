#version 330
in vec2 outTextCord;
 uniform sampler2D tex1;
 uniform vec2 screenSize;
 out vec4 outputColor;

 //for gaussian blur:
 float PixOffset[11] = float[](0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0);
 float Weight[11]=float[](0.103686,0.100287,0.0907433,0.0768126,0.0608271,0.0450618,0.0312297,0.0202476,0.0122808,0.00696828,0.0036989);
void main()
{

	//pass 2:
	float dy=1.0/screenSize.y;
	vec4 sum=texture(tex1,outTextCord)*Weight[0];
	for(int  i=1;i<11;i++)
	{
	sum+=texture(tex1,outTextCord+vec2(0.0,PixOffset[i])*dy)*Weight[i];
	sum+=texture(tex1,outTextCord-vec2(0.0,PixOffset[i])*dy)*Weight[i];

	}
	//vec4 texColor=texture(tex1, outTextCord);
	outputColor=sum;//
	


}
