#game project
include_directories("${PROJECT_SOURCE_DIR}/Install/RaptorEngine/interface")
link_directories("${PROJECT_SOURCE_DIR}/Install/RaptorEngine")
set (GAME_SOURCE
	Player.h
	Player.cpp
	Scene.cpp
	Scene.h
	DbgOut.h
	Input.h
	main.cpp
	)

add_executable (ResidentEvil ${GAME_SOURCE})

target_link_libraries(ResidentEvil RaptorEngine)
add_dependencies(ResidentEvil RaptorEngine)

#copy game binary and assets to install folder

install (TARGETS ResidentEvil RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/Install/ResidentEvil/)

install (DIRECTORY models DESTINATION ${PROJECT_SOURCE_DIR}/Install/ResidentEvil/)
install (DIRECTORY textures DESTINATION ${PROJECT_SOURCE_DIR}/Install/ResidentEvil/)

#copy required dlls
install (FILES 
	${PROJECT_SOURCE_DIR}/Install/assimp/Assimp32.dll	${PROJECT_SOURCE_DIR}/Install/GL/freeglut.dll
	DESTINATION ${PROJECT_SOURCE_DIR}/Install/ResidentEvil)
#get the shaders
install (DIRECTORY ${PROJECT_SOURCE_DIR}/Install/RaptorEngine/shaders DESTINATION ${PROJECT_SOURCE_DIR}/Install/ResidentEvil/)

