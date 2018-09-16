include_directories(
    ../thirddep/glm-0.9.8
    ../thirddep/glew-2.1.0/include/GL
) 
file(GLOB CXX_FILES *.cpp *.h)
 
add_library(OpenGLEngine STATIC
 ${CXX_FILES} 
 )
 target_link_libraries(OpenGLEngine  ${CMAKE_CURRENT_SOURCE_DIR}/../thirddep/glew-2.1.0/lib/glew32.lib)
 target_link_libraries(OpenGLEngine  opengl32.lib)
 set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/OpenGL)