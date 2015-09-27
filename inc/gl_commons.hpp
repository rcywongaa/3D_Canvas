#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
 
#include <iostream>
#include <fstream>
#include <vector>

#include "config.hpp"

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

