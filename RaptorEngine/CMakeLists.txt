# Create a library called RaptorEngine 
# The extension is already found. Any number of sources could be listed here.

include_directories("${PROJECT_SOURCE_DIR}/Install/SOIL/include")

set (ENGINE_SOURCE
	FBO.cpp
	Mesh.cpp
	PhysicalBody.cpp
	RaptorEngine.cpp
	Renderer.cpp
	Shader.cpp
	Texture.cpp
	TextureViewer.cpp
	UtilityFunctions.cpp
	${PROJECT_SOURCE_DIR}/Install/GL/glew.c
	)

add_library (RaptorEngine ${ENGINE_SOURCE})
target_link_libraries(RaptorEngine freeglut assimp SOIL)
#copy artifact to Install folder
install (TARGETS RaptorEngine ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/Install/RaptorEngine/)
install(DIRECTORY interface DESTINATION ${PROJECT_SOURCE_DIR}/Install/RaptorEngine/)
install(DIRECTORY shaders DESTINATION ${PROJECT_SOURCE_DIR}/Install/RaptorEngine/)