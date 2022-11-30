file(GLOB SRC_FILE *.cpp)
add_library(renderer STATIC ${SRC_FILE})
target_link_libraries(renderer Eigen3 stb OpenMP::OpenMP_CXX)
target_include_directories(renderer PUBLIC ${CMAKE_SOURCE_DIR}/include)