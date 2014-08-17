#version 330
 in vec2 outTextCord;
 uniform sampler2D tex1;
 out vec4 outputColor;
float luminance( vec3 color ) {
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
  // return 0.4 * color.r + 0.3 * color.g + 0.3 * color.b;
}
void main()
{
	float LumThresh=0.3;
	vec4 texColor=texture(tex1,outTextCord);
	outputColor=texColor*clamp(luminance(texColor.rgb) - LumThresh,0.0,1.0 )*(1.0/(1.0-LumThresh));
        //outputColor=texColor * 0.5;
   // else
    //    outputColor=vec4(0.0);
}
