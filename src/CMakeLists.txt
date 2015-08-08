set(proj_src main.cpp)

add_executable(CV ${proj_src})
find_package( OpenCV REQUIRED )
target_link_libraries( CV ${OpenCV_LIBS} )
