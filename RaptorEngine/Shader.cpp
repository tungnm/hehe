#include "Shader.h"

using namespace std;
//this function just read a file into char array and return
char *textFileRead(const char* shaderFile) {

	FILE* fp = fopen(shaderFile, "rb");
	if ( fp == NULL ) { return NULL; }
	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	//this is important, the sample file in the book used: buf[size]=' ' which will not work
	buf[size]='\0';
	fclose(fp);

	return buf;
}


	//	GLuint MVHandle,MVPHandle,normalMHandle,cameraMovementHandle;//handleID to view, projection, normal matrix
	//	GLuint modeHandle,isCubeMapHandle;//handle to mode uniform
	void ShaderPass::SendFloatToUni(string uniformName, float value)
	{
		int handle=glGetUniformLocation(pHandle,uniformName.c_str());
		glUniform1f(handle,value);
	}
	void ShaderPass::SendVec3ToUni(string uniformName, cml::vector3f v)
	{
		int handle=glGetUniformLocation(pHandle,uniformName.c_str());
		glUniform3f(handle,v[0],v[1],v[2]);
	}
	void ShaderPass::SendVec3ToUni(string uniformName, GLfloat v1,GLfloat v2,GLfloat v3)
	{
		int handle=glGetUniformLocation(pHandle,uniformName.c_str());
		glUniform3f(handle,v1,v2,v3);
	}
	void ShaderPass::SendMat4ToUni(string uniformName, cml::matrix44f_c* mat)
	{
		int handle=glGetUniformLocation(pHandle,uniformName.c_str());
		glUniformMatrix4fv(handle,1,false,mat->data());
	}
	void ShaderPass::SendMat3ToUni(string uniformName, cml::matrix33f_c* mat)
	{
		int handle=glGetUniformLocation(pHandle,uniformName.c_str());
		glUniformMatrix3fv(handle,1,false,mat->data());

	}
		void ShaderPass::SendVec2ToUni(string uniformName, GLfloat v1,GLfloat v2)
	{
		int handle=glGetUniformLocation(pHandle,uniformName.c_str());
		glUniform2f(handle,v1,v2);
	}
	std::string intToString(int i)
	{
		std::stringstream ss;
		std::string s;
		ss << i;
		s = ss.str();

		return s;
	}
	void ShaderPass::SendFloatArrayToUni(string uniformArrayName,int arrayCount,GLfloat* floatArray)
	{
		for(int i=0;i<arrayCount;i++)
		{

			string handleName=uniformArrayName+"["+intToString(i)+"]";
			int handle=glGetUniformLocation(pHandle,handleName.c_str());
			glUniform1f(handle,floatArray[i]);
		}

	}
	void ShaderPass::SendIntToUni(string uniformName, int value)
	{
		int handle=glGetUniformLocation(pHandle,uniformName.c_str());
		glUniform1i(handle,value);
	}
	ShaderPass::ShaderPass()
	{

	}
	GLuint ShaderPass::GetProgramHandle()
	{
		return pHandle;
	}

	void ShaderPass::Activate()
	{
		glUseProgram(pHandle);
	}
	void ShaderPass::InitShader(const char* vShaderFile, const char* fShaderFile)
	{
		std::cout<<"Loading"<<vShaderFile<<"\n";
		//initialize glew, required for every shader program
		GLenum err = glewInit();
		//read Vertex Shader
		//create new shader object, this function return a unique identifier for the shader, which could use to reference the shader
		GLuint vertShader= glCreateShader(GL_VERTEX_SHADER);
		vertexHandle=vertShader;

		if (vertShader==0)
		{
			std::cout<<"Error: Cannot create vertex shader";
			exit(1);
		}
		//copy source code into shader object:
		const GLchar * shaderCode = textFileRead(vShaderFile);
		const GLchar* codeArray[] = {shaderCode};
		glShaderSource( vertShader, 1, codeArray, NULL );
		//compile vertex shader:
		glCompileShader(vertShader);
		//verify the shader compilation status:
		GLint result;
		glGetShaderiv(vertShader,GL_COMPILE_STATUS,&result);
		if(result==GL_FALSE)
		{
			cout<<"Vertex Shader compilation failed:\n";
			//the code below is to provide the detail of failure:
			GLint logLen;
			glGetShaderiv( vertShader, GL_INFO_LOG_LENGTH, &logLen );
			if( logLen > 0 )
			{
				char * log = (char *)malloc(logLen);
				GLsizei written;
				glGetShaderInfoLog(vertShader, logLen, &written, log);
				fprintf(stderr, "Shader log:\n%s", log);
				free(log);
			}
		}
		//=================================end of vertex===============================
		//create, load source and compile fragment shader:
		//read Fragment Shader:
		//create new shader object, this function return a unique identifier for the shader, which could use to reference the shader
		std::cout<<"Loading"<<fShaderFile<<"\n";
		GLuint fragShader= glCreateShader(GL_FRAGMENT_SHADER);
		fragmentHandle=fragShader;

		if (fragShader==0)
		{
			std::cout<<"Error: Cannot create fragment shader";
			exit(1);
		}
		//copy source code into shader object:
		const GLchar * fshaderCode = textFileRead(fShaderFile);
		const GLchar* fcodeArray[] = {fshaderCode};
		glShaderSource( fragShader, 1, fcodeArray, NULL );
		//compile vertex shader:
		glCompileShader(fragShader);
		//verify the shader compilation status:
		GLint fresult;
		glGetShaderiv(fragShader,GL_COMPILE_STATUS,&fresult);
		if(fresult==GL_FALSE)
		{
			cout<<"Fragment Shader compilation failed:\n";
			//the code below is to provide the detail of failure:
			GLint logLen;
			glGetShaderiv( vertShader, GL_INFO_LOG_LENGTH, &logLen );
			if( logLen > 0 )
			{
				char * log = (char *)malloc(logLen);
				GLsizei written;
				glGetShaderInfoLog(vertShader, logLen, &written, log);
				fprintf(stderr, "Shader log:\n%s", log);
				free(log);
			}
		}
		//================================End of fragment============================

		//create program:
		//generate program handle, similar to vertex and fragmentShader handle:
		pHandle= glCreateProgram();
		if(pHandle==0)
		{
			cout<<"Error: Cannot create program for shader";
			exit(1);
		}
		//attach shaders to program:
		glAttachShader(pHandle, vertShader);
		glAttachShader(pHandle, fragShader);
		//link the program:
		glLinkProgram(pHandle);
		//after linking the program, we could delete the shader files:
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);

		//verify link status of program:
		GLint status;
		glGetProgramiv( pHandle, GL_LINK_STATUS, &status );
		if( status==GL_FALSE ) //iflink failed then print the failed  status
		{
			fprintf( stderr, "Failed to link shader program!\n" );
			GLint logLen;
			glGetProgramiv(pHandle, GL_INFO_LOG_LENGTH, 
				&logLen);
			if( logLen > 0 )
			{
				char * log = (char *)malloc(logLen);
				GLsizei written;
				glGetProgramInfoLog(pHandle, logLen, 
					&written, log);
				fprintf(stderr, "Program log: \n%s", log);
				free(log);
			}
		}
		
	}

	
//this function create shader object, load shader source and compile a shader for vert and frag shader
	void ShaderPass::Unload()
	{
		glDeleteShader(vertexHandle);
		glDeleteShader(fragmentHandle);
		glDeleteProgram(pHandle);
	}

int textFileWrite(char *fn, char *s) {

	FILE *fp;
	int status = 0;

	if (fn != NULL) {
		fp = fopen(fn,"w");

		if (fp != NULL) {

			if (fwrite(s,sizeof(char),strlen(s),fp) == strlen(s))
				status = 1;
			fclose(fp);
		}
	}
	return(status);
}

void ShaderPass::SendViewSpaceTransformtoUni(cml::matrix44f_c* MV,cml::matrix44f_c* MVP, cml::matrix33f_c* Normal)
{	
	//sending modelView and projection matrix as uniform to vertex shader:
	SendMat4ToUni("MV",MV);//pass in model matrix to transform to
	//world space
	SendMat4ToUni("MVP",MVP);
	SendMat3ToUni("normalTransform",Normal);//transformation of normal to 

}

void ShaderPass::SendWorldSpaceTransformtoUni(cml::matrix44f_c* M,cml::matrix44f_c* MVP, cml::matrix33f_c* Normal)
{
	SendMat4ToUni("M",M);//pass in model matrix to transform to
	//world space
	SendMat4ToUni("MVP",MVP);
	SendMat3ToUni("normalTransform",Normal);//transformation of normal to 
}

