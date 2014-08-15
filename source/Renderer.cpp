#include "Renderer.h"
#include "TextureViewer.h"

//------------------------------------------HASH TABLE FOR TEXT RENDERING
string alphabet[8]={"ABCDEFGH",
					"IJKLMNOP",
					"QRSTUVWX",
					"YZabcdef",
					"ghijklmn",
					"opqrstuv",
					"wxyz0123",
					"456789:.:"
};	

unordered_map<char, cml::vector2f> alphabetTextureMap;
void BuildAlphabetHashMap()
{
	//map each cell on the texture or each letter, to a corresponding u,v coordinate
	for(int i=0;i<8;i++)
		for(int j=0;j<8;j++)
		{
			alphabetTextureMap[alphabet[i][j]]=cml::vector2f((float)j/8.0f,(float)(7-i)/8.0f);
		}
}





//text size:
	const GLfloat INGAME_TEXT_SIZE=0.019;
	const GLfloat MENU_TEXT_SIZE=0.03;
	//string position:
	cml::vector2f TEXT_POS(-0.65,0.45);
	GLfloat TEXT_SPACE_X=0.03f;
	GLfloat MENU_TEXT_SPACE_X=0.05f;
	GLfloat TEXT_SPACE_Y=0.15f;

	void Renderer::UpdateLightMatrix()
	{
		_lightView.identity();
	_lightModel.identity();
	cml::matrix_look_at_RH(_lightView,lightPos[0],lightPos[1],lightPos[2],lightTarget[0],lightTarget[1],lightTarget[2],0.0f,1.0f,0.0f);
	cml::matrix_set_translation(_lightModel,lightPos[0],lightPos[1],lightPos[2]);

	}

void Renderer::InitLightMatrix()
{
	_lightProj.identity();
	
	cml::matrix_perspective_yfov_RH(_lightProj, cml::rad(45.0f), (GLfloat)(W_WIDTH)/(GLfloat)(W_HEIGHT), Z_NEAR,Z_FAR, cml::ZClip::z_clip_zero);
	UpdateLightMatrix();
}


void Renderer::InitCameraAndMatrix()
{
	_projection.identity();

	cml::matrix_perspective_yfov_RH(_projection, cml::rad(45.0f), (GLfloat)(W_WIDTH)/(GLfloat)(W_HEIGHT),Z_NEAR, Z_FAR, cml::ZClip::z_clip_zero);

	//	_cameraPosition.set(-5.f,5.0f,0.0f);

	//Initialize camera direction and camera tagent need to be perpendicular to each other
	//_cameraDirection.set(1.0f,-1.0f,0.0f);
	//_cameraTagent.set(0.0f,0.0f,1.0f);

	//	cml::matrix_look_at_RH(_cameraSetup,_cameraPosition[0],_cameraPosition[1],_cameraPosition[2],0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
	//initliaze light:
	InitLightMatrix();

}
void Renderer::SetCameraPosition(cml::vector3f camPos, cml::vector3f camTarget)
{

	_cameraPosition=camPos;
	cml::matrix_look_at_RH(_cameraSetup,_cameraPosition[0],_cameraPosition[1],_cameraPosition[2],camTarget[0], camTarget[1],camTarget[2],0.0f,1.0f,0.0f);

}

void Renderer::UpdateCamera()
{
	cml::matrix_look_at_RH(_cameraSetup,_cameraPosition[0],_cameraPosition[1],_cameraPosition[2],_cameraPosition[0]+_cameraDirection[0],_cameraPosition[1]+_cameraDirection[1],_cameraPosition[2]+_cameraDirection[2],0.0f,1.0f,0.0f);
}
void Renderer::SetLightTarget(GLfloat x,GLfloat y,GLfloat z)
{
	lightTarget.set(x,y,z);
	UpdateLightMatrix();
}

void Renderer::setLightPos(GLfloat x,GLfloat y,GLfloat z)
{
	lightPos.set(x,y,z,1.0);
	UpdateLightMatrix();
}

void Renderer::StartUp()
{
	//initialize all manager singletons
	textureMan=TextureManager::GetInstance();
	textureMan->StartUp();
	isSsao=1;
	hasSkybox=false;
	isBloom=true;
	lastVaoHandle=-1;
	lastUpdateTick=0;
	textureOffset=1.0;

	meshMan=MeshManager::GetInstance();
	meshMan->StartUp();


	//load shader and set up static uniforms
	InitShader();
	InitCameraAndMatrix();
	LoadEngineResource();
	InitFBOs();
	lightPos.set(3.0f,20.5f,-16.0f,1.0f);

	InitStaticUniforms();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);

	BuildAlphabetHashMap();//need this for rendering text
	//scene
//	fire1=new ParticleSystem(0.2,cml::vector3f(0.0,0.0,0.0 /*2.0f,0.0f,-3.0f*/),particleSystemType::fire,0.45f);
//	fire1->Start();
	/*glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc (GL_ONE, GL_ONE);*/

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
}

GLfloat gauss(float x, float sigma2 )
{

	double coeff = 1.0 / (2.0 * 3.14159265 * sigma2);
	double expon = -(x*x) / (2.0 * sigma2);
	return (float) (coeff*exp(expon));
}

void Renderer::SetupWeight(int kernelSize, GLfloat sigma2 )
{
	GLfloat *weights;
	weights=new GLfloat[kernelSize];
	GLfloat sum;//for gaussian blur:
	// Compute and sum the weights
	weights[0] = gauss(0,sigma2); // The 1-D Gaussian function
	sum = weights[0];
	for( int i = 1; i < kernelSize; i++ ) {
		weights[i] = gauss(i, sigma2);
		sum += 2 * weights[i];
	}

	ofstream myOfile;
	myOfile.open ("blur.txt");
	// Normalize the weights and print result to text file:
	for( int i = 0; i < kernelSize; i++ ) {
		//sn////printf(uniName, 20, "Weight[%d]", i);

		weights[i]=weights[i] / sum;
		myOfile << 	weights[i]<< ",";
	}
	myOfile<<"\n\n\n";
	//print the kernel to a text file:
	for(int i=0;i<kernelSize;i++)
		myOfile<<i<<".0, ";
	myOfile.close();

	delete [] weights;
	weights=0;
}

void Renderer::ShutDown()
{
	meshMan->ShutDown();
	textureMan->ShutDown();

	//delete all textures in G-Buffer:
	shadowFBO1.Dellocate();
	gbuffer1.Dellocate();

	//delete all textures in FBOs
	for(std::unordered_map<string, SingleTexFBO*>::iterator iter =  _fboList.begin(); iter != _fboList.end(); ++iter)
	{
		iter->second->Dellocate();
		delete iter->second;
	}
	//delete texture viewers:
	for(std::unordered_map<string, TextureViewer*>::iterator iter =  _textureViewList.begin(); iter != _textureViewList.end(); ++iter)
	{
		delete iter->second;
	}

	UnloadShaders();
	delete fire1;
}

void Renderer::LoadTexture(string textureKey)
{
	string fileName=textureKey;
	if( !textureMan->ExistTexture(fileName))//only load if not loaded before
	{

		char* a=new char[fileName.size()+1];
		int i;
		for(i=0;i<fileName.size();i++)
		{
			a[i]=fileName[i];
		}
		a[i]='\0';
		textureMan->LoadSingleImage(a);

	}
}

void Renderer::LoadMesh(string meshKey)
{
	//check if mesh is already there, if so dont have to load
	if(!meshMan->IsMeshExist(meshKey))
	{
		meshMan->LoadObjFile(meshKey);

	}
}
void Renderer::RenderSkyBox()
{

	BuildGBufferSkyBox.Activate();
	UpdateMatrices(skyBox.GetModelMatrix());
	BuildGBufferSkyBox.SendWorldSpaceTransformtoUni(&skyBox.GetModelMatrix(), &_modelViewProj, &_normalTransform);
	BuildGBufferSkyBox.SendVec3ToUni("camPos",_cameraPosition[0],_cameraPosition[1],_cameraPosition[2]);

	//code to draw skybox
	glFrontFace(GL_CW);
	MeshRenderData mesh=meshMan->GetMeshRenderData("box.obj");
	// bind VAO
	glBindVertexArray(mesh.VAOHandle);
	// draw
	glDrawElements(GL_TRIANGLES,mesh.nFaces*3,GL_UNSIGNED_INT,0);

	glFrontFace(GL_CCW);


}
void Renderer::InitShader()
{
	//color dot for particle system
	//	BuildGBufferFire.InitShader("shaders\\fireVert.glsl","shaders\\fireVertGFrag.glsl");
	//	BuildGBufferSkyBox.InitShader("shaders\\skyBoxVert.glsl","shaders\\skyBoxGFrag.glsl"); 

	TextShader.InitShader("shaders\\TextVert.glsl","shaders\\TextFrag.glsl");
	quadShader.InitShader("shaders\\quadVert.glsl","shaders\\quadFrag.glsl");
	BuildGBufferNormalMap.InitShader("shaders\\normalMapVert.glsl","shaders\\normalMapGFrag.glsl");

	//	BuildGBufferWater.InitShader("shaders\\waterVert.glsl","shaders\\waterGBufferFrag.glsl");

	ProcessGBufferPass.InitShader("shaders\\quadVert.glsl","shaders\\defferedNormalSSAO.glsl");
	instancedVert.InitShader("shaders\\instancedDrawingVert.glsl","shaders\\instancedFrag.glsl");

	//quad shader for bloom:
		verticalBlurPass.InitShader("shaders\\quadVert.glsl","shaders\\verticalBlurFrag.glsl");
	horizontalBlurPass.InitShader("shaders\\quadVert.glsl","shaders\\horizontalBlurFrag.glsl");
	brightExtractionPass.InitShader("shaders\\quadVert.glsl","shaders\\extractFrag.glsl");
	buildShadowMap.InitShader("shaders\\ShadowVert.glsl","shaders\\ShadowFrag.glsl");

}
void Renderer::UnloadShaders()
{
	BuildGBufferFire.Unload();
	BuildGBufferSkyBox.Unload(); 
	quadShader.Unload();
	BuildGBufferNormalMap.Unload();

	BuildGBufferWater.Unload();

	ProcessGBufferPass.Unload();
	//quad shader for bloom:
	verticalBlurPass.Unload();
	horizontalBlurPass.Unload();
	brightExtractionPass.Unload();
	buildShadowMap.Unload();
}
void Renderer::LoadLetterMesh()
{
	ifstream myfile ("letterMeshList.txt"); 
	string line;
	//------------------------------------------------------------------------------------------------------------
	while(!myfile.eof())
	{
		getline (myfile, line);

		meshMan->LoadObjFile(line);
	}
	myfile.close();
}
void Renderer::LoadEngineResource()
{
	//	LoadLetterMesh();

	//	LoadTexture("textures\\random_normals.png");
	//deferredSecondPass.SendIntToUni("randomTex",_textureList["noise"]->GetID());
	meshMan->LoadObjFile("quad.obj");
	meshMan->LoadObjFile("box.obj");
	//	meshMan->Load("particle2.obj");
	//LoadCubemap("textures\\stormy\\tforward.png","textures\\stormy\\tbackward.png","textures\\stormy\\tup.png","textures\\stormy\\tdown.png","textures\\stormy\\tright.png","textures\\stormy\\tleft.png");
	//LoadCubemap("textures\\nightsky\\nightsky_west.bmp","textures\\nightsky\\nightsky_east.bmp","textures\\nightsky\\nightsky_up.bmp","textures\\nightsky\\nightsky_down.bmp","textures\\nightsky\\nightsky_south.bmp","textures\\nightsky\\nightsky_north.bmp");

	//	meshMan->Load("water.obj");
	//	waterModel.meshKey="water.obj";
	//	waterModel.SetScale(0.2,0.02,0.2);
	//	waterModel.Translate(0.0,-1.0,0.0);

}
void Renderer::InitStaticUniforms()
{

	//--NOTE: need to activate shader before sending data to the shader
	//	LoadTexture("textures\\fire2.jpg");

	//set fire texture handle for fire shader
	//	BuildGBufferFire.Activate();
	//	BuildGBufferFire.SendIntToUni("fireTexture",textureMan->GetTextureID ("textures\\fire2.jpg"));

	//cubemap shader
	//	BuildGBufferSkyBox.Activate();
	//	BuildGBufferSkyBox.SendIntToUni("CubeMapTex",textureMan->GetCubeMapID());//set texture for skybox and reflective objs once

	//	BuildGBufferWater.Activate();
	//	BuildGBufferWater.SendIntToUni("CubeMapTex",textureMan->GetCubeMapID());
	LoadTexture("letters.png");
	
	TextShader.Activate();
	TextShader.SendIntToUni("tex1",textureMan->GetTextureID("letters.png") );
	diffuseTexturePass.Activate();

	diffuseTexturePass.SendVec3ToUni("lightPos",3.0,6.0,3.0);

	//set up uniforms for deferred shader, from G-Buffer
	ProcessGBufferPass.Activate(); 
	ProcessGBufferPass.SendIntToUni("gPosTex",gbuffer1.GetGPosTexHandle());
	ProcessGBufferPass.SendIntToUni("gNormTex",gbuffer1.GetGNormTexHandle());
	ProcessGBufferPass.SendIntToUni("gDiffuseTex",gbuffer1.GetGDiffuseTexHandle());
	ProcessGBufferPass.SendIntToUni("gTanTex",gbuffer1.GetGTanTexHandle());
	ProcessGBufferPass.SendIntToUni("gNormalMapTex",gbuffer1.GetGNormalMapTexHandle());
	ProcessGBufferPass.SendMat4ToUni("camProj",&_projection);
	ProcessGBufferPass.SendIntToUni("shadowMap",shadowFBO1.GetTextureHandle());
	LoadTexture("random_normals.png");
	ProcessGBufferPass.SendIntToUni("rnm",textureMan->GetTextureID("random_normals.png"));


	TextureViewer temp;
	temp.SetNewDimension(0.0f,0.0f,1.0f);
	ProcessGBufferPass.SendMat4ToUni("MV",temp.GetModelMatrix());
	instancedVert.Activate();
	instancedVert.SendMat4ToUni("P",&_projection);
	instancedVert.SendMat4ToUni("V",&_cameraSetup);




	deferredSecondPass.Activate();
	deferredSecondPass.SendIntToUni("gNormTex",gbuffer1.GetGNormTexHandle());
	deferredSecondPass.SendMat4ToUni("MV",_textureViewList["fullScreen"]->GetModelMatrix());
	cml::matrix33f_c projI;
	cml::matrix_linear_transform(projI,_projection);//this function extract the top left 3-3 matrix from _cameraSetup to _normalTransform
	projI.inverse();
	deferredSecondPass.SendMat3ToUni("ProjectionMatrixInverse",&projI);

	//bloom output of extracted highlight pass is the input of the first blur pass
	brightExtractionPass.Activate();
	brightExtractionPass.SendIntToUni("tex1",_fboList["ssaoOutput"]->GetTextureHandle());//get texture from pass1
	verticalBlurPass.Activate();//normal texture shader, will draw texture as origin
	verticalBlurPass.SendIntToUni("tex1",_fboList["extractedHightLight"]->GetTextureHandle());
	verticalBlurPass.SendVec2ToUni("screenSize",_fboList["extractedHightLight"]->_width,_fboList["extractedHightLight"]->_height);

//	output of first blur is input of second blur pass. this pass also take in the original ssao output
//	to add them together
	horizontalBlurPass.Activate();
	horizontalBlurPass.SendIntToUni("tex1",_fboList["firstBlur"]->GetTextureHandle());
	horizontalBlurPass.SendIntToUni("texOrigin",_fboList["ssaoOutput"]->GetTextureHandle());
	horizontalBlurPass.SendVec2ToUni("screenSize",_fboList["firstBlur"]->_width,_fboList["firstBlur"]->_height);
}

Appearance* Renderer::GetAppeance(string meshKey, string diffTex, string normTex, cml::vector3f pos, cml::vector3f scale, GLfloat rot)
{
	LoadMesh(meshKey);
	LoadTexture(diffTex);
	LoadTexture(normTex);
	Appearance* newOBJ= new Appearance(meshKey);
	_normalMapList.push_back(newOBJ);

	newOBJ->SetTexture(MapType::diffuse,diffTex);
	newOBJ->SetTexture(MapType::normal,normTex);

	newOBJ->SetScale(scale[0],scale[1],scale[2]);
	newOBJ->Translate(pos[0],pos[1],pos[2]);
	newOBJ->Rotate(rot);
	return newOBJ;
}


void Renderer::AddSolidColorOBJ(string meshKey,cml::vector3f color,cml::vector3f pos, cml::vector3f scale, GLfloat rot)
{
	LoadMesh(meshKey);
	_solidColorList.push_back(new Appearance(meshKey));
	_solidColorList[_solidColorList.size()-1]->SetColor(color[0],color[1],color[2]);

	_solidColorList[_solidColorList.size()-1]->SetScale(scale[0],scale[1],scale[2]);
	_solidColorList[_solidColorList.size()-1]->Translate(pos[0],pos[1],pos[2]);
	_solidColorList[_solidColorList.size()-1]->Rotate(rot);
}
void Renderer::RenderObj(Appearance* a)
{	

	MeshRenderData*mesh=&(meshMan->GetMeshRenderData(a->meshKey));
	//	if(mesh->VAOHandle!=lastVaoHandle)
	//	{
	//draw the mesh:
	glBindVertexArray(mesh->VAOHandle);
	//	lastVaoHandle=mesh->VAOHandle;
	//	}
	// draw
	glDrawElements(GL_TRIANGLES,mesh->nFaces*3,GL_UNSIGNED_INT,0);
}

void Renderer::UpdateMatrices(cml::matrix44f_c currengModelMatrix)
{	
	_MV=_cameraSetup*currengModelMatrix;
	cml::matrix_linear_transform(_normalTransform,_MV);//transform normal to view space
	//equal to inversing and transposing the to View space matrix
	_normalTransform.inverse();
	_normalTransform.transpose();

	_modelViewProj=_projection*_MV;

}
void Renderer::GoraudRender()
{
	goraudPass.Activate();

	for(int i=0;i<_solidColorList.size();i++)
	{	//send common matrix: MV, MVP...

		////Send MVPLight
		cml::vector3f myColor=_solidColorList[i]->GetColor();
		UpdateMatrices(_solidColorList[i]->GetModelMatrix());
		goraudPass.SendViewSpaceTransformtoUni(&_MV, &_modelViewProj, &_normalTransform);
		goraudPass.SendVec3ToUni("color",myColor[0],myColor[1],myColor[2]);

		RenderObj(_solidColorList[i]);
	}
}

void Renderer::OrientCameraWithMousePos(int xDisplacement, int yDisplacement)
{
	_cameraDirection.set(0.0,0.0,1.0);
	_cameraTagent.set(1.0,0.0,0.0);

	_cameraDirection=cml::rotate_vector(_cameraDirection,OY,(-xDisplacement)*0.005f);

	_cameraTagent=cml::rotate_vector(_cameraTagent,OY,(-xDisplacement)*0.005f);
	_cameraDirection=cml::rotate_vector(_cameraDirection,_cameraTagent,(yDisplacement)*0.005f);
}
void Renderer::ToggleBloom()
{
	if(isBloom) isBloom=0;
	else isBloom=1;
}
void Renderer::ToggleSSAO()
{
	if(isSsao) isSsao=0;
	else isSsao=1;

	ProcessGBufferPass.Activate(); 
	ProcessGBufferPass.SendIntToUni("ssaoOn",isSsao);
}
void Renderer::ShowText(string key)
{
	textList[key]->Visibility=true;
}
	void Renderer::HideText(string key)
	{
		
	textList[key]->Visibility=false;
	}
void Renderer::BuildShadowMap(ShadowFBO* shadowFbo)
{
	//update light matrix:
	//cml::matrix_look_at_RH(_lightView,lightPos[0],lightPos[1],lightPos[2],0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
	//cml::matrix_set_translation(_lightModel,lightPos[0],lightPos[1],lightPos[2]);


	shadowFbo->SetActive();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	buildShadowMap.Activate(); 

	glCullFace(GL_FRONT);
	//for(int i=0;i<_normalMapList.size();i++)
	//{
	//	if(_normalMapList[i]->visible==false) continue;

	//	//Render all objects with Build shadow map shader( in this case only need to pass the MVP to light space for that object)
	//	cml::matrix44f_c MVPL;
	//	MVPL.identity();
	//	MVPL=_lightProj*_lightView*_normalMapList[i]->GetModelMatrix();
	//	buildShadowMap.SendMat4ToUni("MVPLight",&MVPL);
	//	RenderObj(_normalMapList[i]);

	//}
	glCullFace(GL_BACK);

}
void Renderer::LoadCubemap(string forward,string backward,string top, string bottom, string right, string left)
{
	textureMan->LoadCubeMap(forward, backward, top,  bottom,  right,  left);
	skyBox.meshKey="box.obj";
	skyBox.transform._position=cml::vector3f(0.0,0.0,0.0);
	skyBox.SetScale(1.5,1.5,1.5);
	hasSkybox=true;
}
void Renderer::Update(int elapsed)
{
	textureOffset+=0.01f;

	int currentTick=elapsed;
	fire1->Update(currentTick,currentTick-lastUpdateTick);

	lastUpdateTick=currentTick;
}

void Renderer::BeginRendering()
{
	//render scene from light point of view and store depth buffer in ShadowFBO, will be later used for shadow mapping
	BuildShadowMap(&shadowFBO1);

	//Build the G buffer, it will store data to be later retrieve for lighting calculation and all other effect
	gbuffer1.SetActive();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//now render all normal map object in the scene
	BuildGBufferNormalMap.Activate();
	//glEnable(GL_DEPTH_TEST);
}

void Renderer::RenderNormalMapObject(Appearance* appearance)
{
		static string lastTextureID="";
		static string lastNormalMapTexID="";
		//Render all objects with BuildGBuffer shader, writing vertex data to G-Buffer FBO
		//	if(appearance->isNonMoving==false)
		//update current modelView, ModelViewProjection matrix based on the model matrix of the object
		//	{
		UpdateMatrices(appearance->GetModelMatrix());
		//send these matrices to shader uniforms
		BuildGBufferNormalMap.SendViewSpaceTransformtoUni(&_MV, &_modelViewProj, &_normalTransform);
		//	}
		//	else //optimization: use the matrices stored inside nonMoving appearance
		//	BuildGBufferNormalMap.SendViewSpaceTransformtoUni(&appearance->MV, &appearance->MVP, &appearance->normalTrans);

		//update the texture sampler for texture of current object
		string texID=appearance->GetDiffuseMap();
		if(lastTextureID!=texID)
		{
			BuildGBufferNormalMap.SendIntToUni("tex1",textureMan->GetTextureID(texID));
			lastTextureID=texID;
		}
		string normalMapTexID=appearance->GetNormalMap();
		if(lastNormalMapTexID!=normalMapTexID)
		{
			BuildGBufferNormalMap.SendIntToUni("normalMap",textureMan->GetTextureID(normalMapTexID));
			lastNormalMapTexID=normalMapTexID;
		}
		//render the object
		RenderObj(appearance);
}
void Renderer::EndRendering()
{
		FBO::SetDefault(W_WIDTH,W_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//	RenderTextureToQuad(gbuffer1.GetGNormTexHandle());
		/*RenderTextureToQuad(shadowFBO1.GetNormalHandle(),QuadPosition::lowerLeft);
		RenderTextureToQuad(shadowFBO1.GetAlbedoandle(),QuadPosition::lowerRight);*/

		//RenderTextureToQuad( textureMan->GetTextureID("textures\\letters.png"));
		ProcessGBuffer();	
}

void Renderer::AddText(string id, string text, cml::vector2f position, GLfloat scale, GLfloat condensed)
{
	textList[id]=new GUIElement();
	textList[id]->Text=text;textList[id]->OnScreenPos=position;textList[id]->Visibility=true;
	textList[id]->Scale=scale;
	textList[id]->condensedValue=condensed;
}

void Renderer::ChangeTex(string id, string newText)
{
	textList[id]->Text=newText;
}
void Renderer::RenderGUITexts()
{
	TextureViewer letterQuad;//use this as temporary textureviewer to render each letter

	TextShader.Activate();
	std::unordered_map<string, GUIElement*>::iterator iter;
	//quadShader.SendVec3ToUni("textColor",0.9,0.9,0.9);
	for(iter=textList.begin();iter!=textList.end();++iter) //for each gui element in the list
	{
		if (iter->second->Visibility==false) continue;
		string currentText=iter->second->Text;
		int stringSize=currentText.size();
		GLfloat posX=iter->second->OnScreenPos[0];
		GLfloat posY=iter->second->OnScreenPos[1];
		GLfloat scale=iter->second->Scale;
		GLfloat condensed=iter->second->condensedValue;

		//letter->SetScale(MENU_TEXT_SIZE);
		for(int i=0;i< stringSize;i++) //for each letter of that element string
		{
			char cLetter=currentText[i];
			if (cLetter==' ') continue;
			//set model matrix via dimension
			letterQuad.SetNewDimension(posX+(scale-condensed)*2*i,posY,scale);
			
			//set texture:

			//string fileName=cLetter+".obj";//+displayText[i];
			//letter->SetQuadShape(_meshList[fileName]);
			//cml::matrix44f_c scale;
			//quadShader.SendIntToUni("tex1",textureMan->GetTextureID("textures\\letters.png"));
			TextShader.SendMat4ToUni("MV",letterQuad.GetModelMatrix());
			TextShader.SendVec2ToUni("texOffset",alphabetTextureMap[cLetter][0],alphabetTextureMap[cLetter][1]);
			RenderOnScreenQuad();
		}
	}
}
void Renderer::RenderTextureToQuad(int textureHandle)
{
	quadShader.Activate();
	quadShader.SendIntToUni("tex1",textureHandle);//visualize the normal:

	//create a temp texture viewer base on the function parameters just for this function, 
	TextureViewer temp;
	temp.SetNewDimension(0.0f,0.0f,1.0f);
	//	quadShader.SendIntToUni("tex1",textureHandle);//visualize the normal:
	quadShader.SendMat4ToUni("MV",temp.GetModelMatrix());
	RenderOnScreenQuad();

}

void Renderer::RenderOnScreenQuad()
{
	MeshRenderData mesh=meshMan->GetMeshRenderData("quad.obj");
	glBindVertexArray(mesh.VAOHandle);
	glDrawElements(GL_TRIANGLES,mesh.nFaces*3,GL_UNSIGNED_INT,0);

}
void Renderer::StrafeCamRight(GLfloat amount)
{
	_cameraPosition[0]+=amount*_cameraTagent[0];
	_cameraPosition[2]+=amount*_cameraTagent[2];

}
void Renderer::StrafeCamLeft(GLfloat amount)
{
	_cameraPosition[0]-=amount*_cameraTagent[0];
	_cameraPosition[2]-=amount*_cameraTagent[2];
}
void Renderer::MoveCamForward(GLfloat amount)
{
	_cameraPosition[0]+=amount*_cameraDirection[0];
	_cameraPosition[2]+=amount*_cameraDirection[2];

}
void Renderer::MoveCamBackward(GLfloat amount)
{
	_cameraPosition[0]-=amount*_cameraDirection[0];
	_cameraPosition[2]-=amount*_cameraDirection[2];

}
//render a full screen quad taking data from the G-Buffer
void Renderer::ProcessGBuffer()
{

	ProcessGBufferPass.Activate();
	//create a temp texture viewer base on the function parameters just for this function, 

	cml::vector4f lightInView=_cameraSetup*lightPos;
	ProcessGBufferPass.SendVec3ToUni("lightPosViewSpace",lightInView[0],lightInView[1],lightInView[2]);

	//bake and send the matrix to transform to light space:
	cml::matrix44f_c MVPL;
	MVPL.identity();
	//need to reverse the effect of object view matrix already store in there

	cml::matrix44f_c VInverse=_cameraSetup;
	VInverse.inverse();
	MVPL=_lightProj*_lightView*VInverse; //this one will transform MV in shader, to MVPLight
	ProcessGBufferPass.SendMat4ToUni("MVPLight",&MVPL);//pass in model matrix to transform to


	RenderOnScreenQuad();



}


void Renderer::InitFBOs()
{
	//---------------------------------Set Up FBO for bloom effect:
	//

	// These setting is for bloom
	FBO::FBOInfo info;
	info.width=W_WIDTH;info.height=W_HEIGHT;info.hasDepth=true;

	_fboList["ssaoOutput"]=new SingleTexFBO();//store origin render
	_fboList["ssaoOutput"]->SetUp(info,textureMan );

	_fboList["extractedHightLight"]=new SingleTexFBO();//store extracted highlighted 
	//the rest doesn't need depth because of post process:
	info.hasDepth=false;info.width/=4;info.height/=4;
	_fboList["extractedHightLight"]->SetUp(info,textureMan);

	_fboList["firstBlur"]=new SingleTexFBO();//first blur(result of second blur will go to default framebuffer
	_fboList["firstBlur"]->SetUp(info,textureMan);

	//--------------------------------------------------------------

	//only use half resolution for shadow FBO
	info.width=W_WIDTH*2;
	info.height=W_HEIGHT*2;
	shadowFBO1.SetUp(info,textureMan);

	//use full size resolution for G-buffer
	info.width=W_WIDTH;
	info.height=W_HEIGHT;
	gbuffer1.SetUp( info,textureMan);

	//add 2 textureViewers:
	_textureViewList["lowerLeft"]=new TextureViewer(-0.5f,-0.5f,0.45f);//lower left viewer
	_textureViewList["fullScreen"]=new TextureViewer(0.0f,0.0f,1.0f);//full screen
	_textureViewList["lowerRight"]=new TextureViewer(0.5f,-0.5f,0.45f);//lower right viewer

}