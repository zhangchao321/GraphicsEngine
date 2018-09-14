file(GLOB CXX_FILES *.cpp *.h Third/*.h Third/*.cpp)
add_executable(D3D11Engine 
 ${CXX_FILES} 
 )