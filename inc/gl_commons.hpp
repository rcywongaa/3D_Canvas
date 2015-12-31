#ifndef GL_COMMONS_HPP
#define GL_COMMONS_HPP

#define OPENCL

#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
 
#ifdef OPENCL
#include <GL/glx.h>
//#include "CL/cl.hpp" //Use when released
#include "CL/cl.h"
#include "CL/cl_gl.h"
#endif

#include <iostream>
#include <fstream>
#include <vector>

#include "config.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define EPSILON 1e-5

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

#endif
