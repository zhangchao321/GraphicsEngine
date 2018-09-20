set(FRAMEWORK_NAME FrameWorkEngine)
include_directories( 
../thirddep
../thirddep/glew-2.1.0/include
../thirddep/glfw-3.1.1/include 
../thirddep/FreeImage-3.15.4 
../thirddep/glo-0.1.0.0 
../thirddep/glu-0.1.0.0 
../thirddep/gles-2.0 
../thirddep/egl-1.4
)  
file(GLOB FRAMEWORK_SOURCE *.cpp)
file(GLOB FRAMEWORK_INLINE *.inl)
file(GLOB FRAMEWORK_HEADER *.hpp)
file(GLOB FRAMEWORK_MD ../*.md)

add_library(${FRAMEWORK_NAME} STATIC ${FRAMEWORK_SOURCE} ${FRAMEWORK_INLINE} ${FRAMEWORK_HEADER} ${FRAMEWORK_MD})
target_link_libraries(${FRAMEWORK_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../thirddep/glew-2.1.0/lib/glew32.lib)
target_link_libraries(${FRAMEWORK_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}/../thirddep/glfw-3.2.1/lib/glfw3.lib)
target_link_libraries(${FRAMEWORK_NAME}  opengl32.lib)

