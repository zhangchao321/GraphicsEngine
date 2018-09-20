include_directories(
    ../thirddep/glm-0.9.8
    ../thirddep/glew-2.1.0/include/GL
    ../thirddep/glfw-3.2.1/include
    ../OpenGLEngine
) 
file(GLOB CXX_FILES *.cpp *.h)
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE") 
add_executable(FogExp
 ${CXX_FILES} 
 )
 target_link_libraries(FogExp  ${CMAKE_CURRENT_SOURCE_DIR}/../thirddep/glew-2.1.0/lib/glew32.lib)
 target_link_libraries(FogExp  ${CMAKE_CURRENT_SOURCE_DIR}/../thirddep/glfw-3.2.1/lib/glfw3.lib)
 target_link_libraries(FogExp  opengl32.lib)
 target_link_libraries(FogExp  OpenGLEngine)

 set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/OpenGL_Bin)