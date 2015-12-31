set(proj_src
    main.cpp
    gl_commons.cpp
    Vao.cpp
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
add_executable(CV ${proj_src})
find_package( OpenCV REQUIRED )
find_package( OpenGL REQUIRED )
find_package( OpenCL REQUIRED )
find_package( GLEW REQUIRED )
find_package( PkgConfig REQUIRED )
find_package( Boost REQUIRED )
pkg_search_module( GLFW CONFIG REQUIRED glfw3 )
pkg_search_module( ASSIMP CONFIG REQUIRED assimp )
#include_directories( GLFW_INCLUDE_DIRS )
target_link_libraries( CV ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_STATIC_LIBRARIES} ${ASSIMP_LIBRARIES} ${OpenCL_LIBRARIES})
