set(projectName ES200DrawElements)
include_directories(
../thirddep
../thirddep/glew-2.1.0/include
../thirddep/glfw-3.1.1/include 
../thirddep/FreeImage-3.15.4 
../thirddep/glo-0.1.0.0 
../thirddep/glu-0.1.0.0 
../thirddep/gles-2.0 
../thirddep/egl-1.4
../FrameWork
)  
file(GLOB CXX_FILES *.cpp *.h) 
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE") 
add_executable(${projectName}
 ${CXX_FILES} 
 )
 target_link_libraries(${projectName}  ${CMAKE_CURRENT_SOURCE_DIR}/../thirddep/glew-2.1.0/lib/glew32.lib)
 target_link_libraries(${projectName}  ${CMAKE_CURRENT_SOURCE_DIR}/../thirddep/glfw-3.2.1/lib/glfw3.lib)
  target_link_libraries(${projectName}  ${CMAKE_CURRENT_SOURCE_DIR}/../thirddep/FreeImage-3.15.4/win64/FreeImage.lib)
 target_link_libraries(${projectName}  opengl32.lib)
 target_link_libraries(${projectName}  FrameWorkEngine)

 set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/OpenGL_Bin)