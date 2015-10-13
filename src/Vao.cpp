#include "Vao.hpp"

Vao::Vao()
{
    vao_ = 0;
    position_buffer_ = 0;
    index_buffer_ = 0;
    normal_buffer_ = 0;
    uv_buffer_ = 0;
};

bool Vao::init( const std::vector<GLfloat>& positions )
{
    if (positions.size() == 0)
    {
        printf("Position data is empty!\n");
        return false;
    }
    position_data_ = positions;
    //VertexArrayObject stores the VertexBufferObject and VertexAttribArray settings
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    //All following settings modify the vao VAO
    m_NumVertices_ = position_data_.size() / 3;
    glGenBuffers(1, &position_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer_);
    // The second parameter, size, is in number of bytes...
    glBufferData(GL_ARRAY_BUFFER, position_data_.size() * sizeof(GLfloat), position_data_.data(), GL_STATIC_DRAW);
    //Configures the interpretation of triangle_position_buffer by vao_triangle,
    //(attribute_no, size, type, isNormalized, stride, buffer_offset)
    glVertexAttribPointer(POS_ATTR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    //Use VertexAttribute POS_ATTR_INDEX to interpret data stored in triangle_position_buffer
    glEnableVertexAttribArray(POS_ATTR_INDEX);
    //If using VAO, glDisableVertexAttribArray should never be called
    //Additional buffers & attributes may be required for UV, normal, texture, etc.
    //glBindVertexArray(0); //Optional, clear vao_triangle binding so we don't accidentally modify it any further
    //Additional VAOs for other objects may follow
    return true;
};

bool Vao::init(
        const std::vector<GLfloat>& positions,
        const std::vector<GLuint>& indices )
{
    if (indices.size() == 0)
    {
        printf("Index data is empty!\n");
        return false;
    }
    init(positions);
    index_data_ = indices;
    glGenBuffers(1, &index_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data_.size() * sizeof(GLuint), index_data_.data(), GL_STATIC_DRAW);
    return true;
};

void Vao::draw()
{
    if (vao_ < 0)
    {
        fprintf(stderr, "VAO not initialized!\n");
        return;
    }
    glBindVertexArray(vao_);
    if (index_buffer_ == 0) 
    {
        //printf("Drawing without indices\n");
        glDrawArrays(GL_TRIANGLES, 0, m_NumVertices_);
    }
    else 
    {
        //printf("Drawing with %d indices\n", index_data_.size());
        glDrawElements(GL_TRIANGLES, index_data_.size(), GL_UNSIGNED_INT, NULL);
    }
};
