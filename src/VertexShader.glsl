#version 330 core
layout (location = 0) in vec3 vertexPosition_modelspace; //because POS_ATTR_INDEX = 0
uniform mat4 MVP;

void main(){

    // Output position of the vertex, in clip space : MVP * position
    vec4 v = vec4(vertexPosition_modelspace,1); // Transform an homogeneous 4D vector, remember ?
    gl_Position = MVP * v;
}
