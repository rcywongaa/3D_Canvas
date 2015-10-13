#ifndef GL_COMMONS_HPP
#define GL_COMMONS_HPP

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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

#endif
