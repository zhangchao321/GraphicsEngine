include_directories(
    ../thirddep
    ../thirddep/glew-2.1.0/include/GL
    ../thirddep/glfw-3.2.1/include
    ../OpenGLEngine
) 
file(GLOB CXX_FILES *.cpp *.h)
 
add_executable(CombineHdr
 ${CXX_FILES} 
 )
 target_link_libraries(CombineHdr  ${CMAKE_CURRENT_SOURCE_DIR}/../thirddep/glew-2.1.0/lib/glew32.lib)
 target_link_libraries(CombineHdr  ${CMAKE_CURRENT_SOURCE_DIR}/../thirddep/glfw-3.2.1/lib/glfw3.lib)
 target_link_libraries(CombineHdr  opengl32.lib)
 target_link_libraries(CombineHdr  OpenGLEngine)

 set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/OpenGL_Bin)