include_directories(
    ../thirddep/glm-0.9.8
    ../thirddep/glew-2.1.0/include/GL
    ../thirddep/glfw-3.2.1/include
    ../OpenGLEngine
) 
file(GLOB CXX_FILES *.cpp *.h)
 
add_executable(PictureLighter
 ${CXX_FILES} 
 )
 target_link_libraries(PictureLighter  ${CMAKE_CURRENT_SOURCE_DIR}/../thirddep/glew-2.1.0/lib/glew32.lib)
 target_link_libraries(PictureLighter  ${CMAKE_CURRENT_SOURCE_DIR}/../thirddep/glfw-3.2.1/lib/glfw3.lib)
 target_link_libraries(PictureLighter  opengl32.lib)
 target_link_libraries(PictureLighter  OpenGLEngine)

 set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/OpenGL_Bin)