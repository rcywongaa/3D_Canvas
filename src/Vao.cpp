#include "Vao.hpp"

Vao::Vao()
{
    vao_ = 0;
    position_buffer_ = 0;
    index_buffer_ = 0;
    normal_buffer_ = 0;
    uv_buffer_ = 0;
    m_NumVertices_ = 0;
    std::fill_n(center, 3, 0.0f);
    std::fill_n(ka, 3, 0.0f);
    std::fill_n(kd, 3, 0.0f);
    std::fill_n(ks, 3, 0.0f);
    std::fill_n(ke, 3, 0.0f);
    shader_id = 0;
    ambient_id = 0;
    diffuse_id = 0;
    specular_id = 0;
    emissive_id = 0;
    light_position_id = 0;
    light_color_id = 0;
    is_light_ = false;
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
    for (unsigned int i = 0; i < position_data_.size(); i+=3)
    {
        center[0] += position_data_[i] / m_NumVertices_;
        center[1] += position_data_[i+1] / m_NumVertices_;
        center[2] += position_data_[i+2] / m_NumVertices_;
    }
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

bool Vao::init(
        const std::vector<GLfloat>& positions,
        const std::vector<GLuint>& indices,
        const std::vector<GLfloat>& normals )
{
    if (normals.size() == 0)
    {
        printf("Normal data is empty!\n");
        return false;
    }
    init(positions, indices);
    normal_data_ = normals;
    glGenBuffers(1, &normal_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glBufferData(GL_ARRAY_BUFFER, normal_data_.size() * sizeof(GLfloat), normal_data_.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(NORMAL_ATTR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(NORMAL_ATTR_INDEX);
    return true;
};

void Vao::set_shader(GLuint id) { shader_id = id; }

void Vao::set_ambient(GLfloat r, GLfloat g, GLfloat b)
{
    if (shader_id == 0)
    {
        printf("ERROR: Shader must be set first!\n");
        return;
    }
    ka[0] = std::max(std::min(1.0f, r), 0.0f);
    ka[1] = std::max(std::min(1.0f, g), 0.0f);
    ka[2] = std::max(std::min(1.0f, b), 0.0f);
    ambient_id = glGetUniformLocation(shader_id, "ka");
}

void Vao::set_diffuse(GLfloat r, GLfloat g, GLfloat b)
{
    if (shader_id == 0)
    {
        printf("ERROR: Shader must be set first!\n");
        return;
    }
    kd[0] = std::max(std::min(1.0f, r), 0.0f);
    kd[1] = std::max(std::min(1.0f, g), 0.0f);
    kd[2] = std::max(std::min(1.0f, b), 0.0f);
    diffuse_id = glGetUniformLocation(shader_id, "kd");
}

void Vao::set_specular(GLfloat r, GLfloat g, GLfloat b, GLfloat s)
{
    if (shader_id == 0)
    {
        printf("ERROR: Shader must be set first!\n");
        return;
    }
    ks[0] = std::max(std::min(1.0f, r), 0.0f);
    ks[1] = std::max(std::min(1.0f, g), 0.0f);
    ks[2] = std::max(std::min(1.0f, b), 0.0f);
    shininess = s;
    specular_id = glGetUniformLocation(shader_id, "ks");
    shininess_id = glGetUniformLocation(shader_id, "shininess");
}

void Vao::set_emissive(GLfloat r, GLfloat g, GLfloat b)
{
    if (shader_id == 0)
    {
        printf("ERROR: Shader must be set first!\n");
        return;
    }
    ke[0] = std::max(std::min(1.0f, r), 0.0f);
    ke[1] = std::max(std::min(1.0f, g), 0.0f);
    ke[2] = std::max(std::min(1.0f, b), 0.0f);
    emissive_id = glGetUniformLocation(shader_id, "ke");
    if (ke[0] >= EPSILON || ke[1] >= EPSILON || ke[2] >= EPSILON)
    {
        is_light_ = true;
        light_position_id = glGetUniformLocation(shader_id, "light_position");
        light_color_id = glGetUniformLocation(shader_id, "light_color");
    }
}

bool Vao::is_light()
{
    return is_light_;
}

void Vao::draw()
{
    if (shader_id == 0) printf("ERROR: Shader must be set first!\n");
    if (vao_ < 0)
    {
        fprintf(stderr, "VAO not initialized!\n");
        return;
    }
    glBindVertexArray(vao_);
    if (is_light_)
    {
        //TODO: Different types of light
        glUniform3fv(light_position_id, 1, &center[0]);
        glUniform3fv(light_color_id, 1, &ke[0]);
    }
    if (index_buffer_ == 0)
    {
        glDrawArrays(GL_TRIANGLES, 0, m_NumVertices_);
    }
    else
    {
        glUniform3fv(ambient_id, 1, &ka[0]);
        glUniform3fv(diffuse_id, 1, &kd[0]);
        glUniform3fv(specular_id, 1, &ks[0]);
        glUniform3fv(emissive_id, 1, &ke[0]);
        glUniform1f(shininess_id, shininess);
        glDrawElements(GL_TRIANGLES, index_data_.size(), GL_UNSIGNED_INT, NULL);
    }
};
