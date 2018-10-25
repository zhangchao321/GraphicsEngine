cmake_minimum_required(VERSION 2.8)
if (POLICY CMP0020)
  cmake_policy (SET CMP0020 NEW)
endif()

if (POLICY CMP0043)
  cmake_policy (SET CMP0043 NEW)
endif()

if (POLICY CMP0071)
  cmake_policy (SET CMP0071 NEW)
endif()
if (POLICY CMP0053)
  cmake_policy (SET CMP0053 NEW)
endif()
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
)
PROJECT(Engine)
add_subdirectory(VulkanSamples)

add_definitions(-UNICODE -D_UNICODE)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS") 
set(CMAKE_AUTOMOC ON)  
add_subdirectory(D3D11)
add_subdirectory(OpenGL)
add_subdirectory(OpenGLSamples)
