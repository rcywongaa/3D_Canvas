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
        GLuint m_NumVertices_;

    public:
        static const GLuint POS_ATTR_INDEX = 0;
        Vao();
        bool init( const std::vector<GLfloat>& positions );
        bool init(
                const std::vector<GLfloat>& positions,
                const std::vector<GLuint>& indices );
        void draw();
};
