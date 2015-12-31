#version 330 core
layout (location = 0) in vec3 vertex_position_modelspace; //POS_ATTR_INDEX = 0
layout (location = 1) in vec2 vertex_uv; //UV_ATTR_INDEX = 1
layout (location = 2) in vec3 vertex_normal; //NORMAL_ATTR_INDEX = 2
uniform mat4 MVP;
uniform vec3 light_position;
uniform vec3 eye_position;
out vec3 light_dir;
out float light_dist;
out vec3 eye_dir;
out vec3 normal;

bool roughEqual(vec3 a, vec3 b)
{
    return (abs(a.x - b.x) < 0.01 && abs(a.y - b.y) < 0.01 && abs(a.z - b.z) < 0.01);
}

void main()
{
    // Output position of the vertex, in clip space : MVP * position
    vec4 v = vec4(vertex_position_modelspace, 1); // Transform an homogeneous 4D vector, remember ?
    light_dir = normalize(light_position - vertex_position_modelspace);
    light_dist = length(light_position - vertex_position_modelspace);
    normal = normalize(vertex_normal);
    eye_dir = normalize(vertex_position_modelspace - eye_position);
    if (true)
        gl_Position = MVP * v;
    else
        gl_Position = vec4(0, 0, 0, 1);
}
