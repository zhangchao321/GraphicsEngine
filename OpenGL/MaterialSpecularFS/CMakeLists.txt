include_directories(
    ../thirddep/glm-0.9.8
    ../thirddep/glew-2.1.0/include/GL
	../OpenGLEngine
) 
file(GLOB CXX_FILES *.cpp *.h)
 
add_executable(MaterialSpecularFS
 ${CXX_FILES} 
 )
 target_link_libraries(MaterialSpecularFS  ${CMAKE_CURRENT_SOURCE_DIR}/../thirddep/glew-2.1.0/lib/glew32.lib)
 target_link_libraries(MaterialSpecularFS  opengl32.lib)
 target_link_libraries(MaterialSpecularFS  OpenGLEngine)

 set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/OpenGL_Bin)