#include "gl_commons.hpp"

class Vao
{
    private:
        GLuint vao_;
        GLuint position_buffer_;
        GLuint index_buffer_;
        GLuint normal_buffer_;
        GLuint uv_buffer_;
        std::vector<GLfloat> position_data_;
        std::vector<GLuint> index_data_;
        std::vector<GLfloat> normal_data_;
        std::vector<GLfloat> uv_data_;
        GLfloat center[3];
        GLuint m_NumVertices_;
        GLfloat ka[3];
        GLfloat kd[3];
        GLfloat ks[3];
        GLfloat ke[3];
        GLfloat shininess;
        GLuint shader_id;
        GLuint ambient_id;
        GLuint diffuse_id;
        GLuint specular_id;
        GLuint shininess_id;
        GLuint emissive_id;
        GLuint light_position_id;
        GLuint light_color_id;
        bool is_light_;

    public:
        static const GLuint POS_ATTR_INDEX = 0;
        static const GLuint UV_ATTR_INDEX = 1;
        static const GLuint NORMAL_ATTR_INDEX = 2;
        Vao();
        bool init( const std::vector<GLfloat>& positions );
        bool init(
                const std::vector<GLfloat>& positions,
                const std::vector<GLuint>& indices );
        bool init(
                const std::vector<GLfloat>& positions,
                const std::vector<GLuint>& indices,
                const std::vector<GLfloat>& normals );
        void set_shader(GLuint id);
        void set_ambient(GLfloat r, GLfloat g, GLfloat b);
        void set_diffuse(GLfloat r, GLfloat g, GLfloat b);
        void set_specular(GLfloat r, GLfloat g, GLfloat b, GLfloat shininess);
        void set_emissive(GLfloat r, GLfloat g, GLfloat b);
        bool is_light();
        void draw();
};
