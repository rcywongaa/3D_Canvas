#version 330 core
uniform vec3 light_color;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform vec3 ke;
uniform float shininess;
in vec3 light_dir;
in float light_dist;
in vec3 normal;
in vec3 eye_dir;
out vec4 color;

bool roughEqual(vec3 a, vec3 b)
{
    return (abs(a.x - b.x) < 0.01 && abs(a.y - b.y) < 0.01 && abs(a.z - b.z) < 0.01);
}

void main()
{
    //normal are dependent on the MVP matrix
    if (true)
    {
        vec3 ref_dir =  reflect(light_dir, normal);
        float cos_alpha = clamp(dot(ref_dir, eye_dir), 0, 1);
        vec3 color3 = vec3(ke);
        color3 += light_color * ka; //ambient
        color3 += light_color * clamp(dot(normal, light_dir) * kd, 0, 1); //diffuse
        color3 += light_color * pow(cos_alpha, shininess) * ks; //specular
        color3 /= light_dist * light_dist;
        color = vec4(color3, 1);
    }
    else
    {
        color = vec4(1.0, 0.0, 0.0, 1.0);
    }
}
