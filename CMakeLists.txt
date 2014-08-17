# The name of our project is "RES_REMAKE". CMakeLists files in this project can 
# refer to the root source directory of the project as ${RES_REMAKE_SOURCE_DIR} and 
# to the root binary directory of the project as ${RES_REMAKE_BINARY_DIR}. 
cmake_minimum_required (VERSION 2.6) 
project (RES_REMAKE) 

include_directories("${PROJECT_SOURCE_DIR}/Install/GL/include")
include_directories("${PROJECT_SOURCE_DIR}/Install/")
link_directories("${PROJECT_SOURCE_DIR}/Install/GL/lib")
link_directories("${PROJECT_SOURCE_DIR}/Install/SOIL/lib")
link_directories("${PROJECT_SOURCE_DIR}/Install/assimp/lib")

# Recurse into the "RaptorEngine" and "Game" subdirectories. This does not actually 
# cause another cmake executable to run. The same process will walk through 
# the project's entire directory structure. 
add_subdirectory (RaptorEngine) 
add_subdirectory (Game)