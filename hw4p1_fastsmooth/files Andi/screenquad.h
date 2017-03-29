#pragma once

#include "icg_helper.h"

class ScreenQuad {

private:
    GLuint vertex_array_id_;        // vertex array object
    GLuint program_id_;             // GLSL shader program ID
    GLuint vertex_buffer_object_;   // memory buffer
    GLuint texture_id_1;            // texture ID
    GLuint texture_id_2;

    float screenquad_width_;
    float screenquad_height_;

    int max_kernel;

public:
    void Init(float screenquad_width , float screenquad_height ,
              GLuint texture_1 , GLuint texture_2) {

        // set screenquad size
        this->screenquad_width_ = screenquad_width;
        this->screenquad_height_ = screenquad_height;

        int minimum = min(screenquad_width , screenquad_height);
        this->max_kernel = minimum % 2 == 1 ? minimum : minimum - 1;

        // compile the shaders
        program_id_ = icg_helper::LoadShaders("screenquad_vshader.glsl" ,
                                              "screenquad_fshader.glsl");
        if (!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex Array
        glGenVertexArrays(1 , &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates
        {
            const GLfloat vertex_point[] = { /*V1*/ -1.0f , -1.0f , 0.0f ,
                    /*V2*/ +1.0f , -1.0f , 0.0f ,
                    /*V3*/ -1.0f , +1.0f , 0.0f ,
                    /*V4*/ +1.0f , +1.0f , 0.0f};
            // buffer
            glGenBuffers(1 , &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER , vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER , sizeof(vertex_point) ,
                         vertex_point , GL_STATIC_DRAW);

            // attribute
            GLuint vertex_point_id = glGetAttribLocation(program_id_ , "vpoint");
            glEnableVertexAttribArray(vertex_point_id);
            glVertexAttribPointer(vertex_point_id , 3 , GL_FLOAT , DONT_NORMALIZE ,
                                  ZERO_STRIDE , ZERO_BUFFER_OFFSET);
        }

        // texture coordinates
        {
            const GLfloat vertex_texture_coordinates[] = { /*V1*/ 0.0f , 0.0f ,
                    /*V2*/ 1.0f , 0.0f ,
                    /*V3*/ 0.0f , 1.0f ,
                    /*V4*/ 1.0f , 1.0f};

            // buffer
            glGenBuffers(1 , &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER , vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER , sizeof(vertex_texture_coordinates) ,
                         vertex_texture_coordinates , GL_STATIC_DRAW);

            // attribute
            GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_ ,
                                                                 "vtexcoord");
            glEnableVertexAttribArray(vertex_texture_coord_id);
            glVertexAttribPointer(vertex_texture_coord_id , 2 , GL_FLOAT ,
                                  DONT_NORMALIZE , ZERO_STRIDE ,
                                  ZERO_BUFFER_OFFSET);
        }

        // load/Assign texture
        this->texture_id_1 = texture_1;
        glBindTexture(GL_TEXTURE_2D , texture_id_1);
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_CLAMP_TO_EDGE);
        GLuint tex_id = glGetUniformLocation(program_id_ , "tex");
        glUniform1i(tex_id , 0 /*GL_TEXTURE0*/);

        this->texture_id_2 = texture_2;
        glBindTexture(GL_TEXTURE_2D , texture_id_2);
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_CLAMP_TO_BORDER);
        GLuint velTex_id = glGetUniformLocation(program_id_ , "tmp_tex");
        glUniform1i(velTex_id , 1 /*GL_TEXTURE1*/);

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1 , &vertex_buffer_object_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1 , &vertex_array_id_);
        glDeleteTextures(1 , &texture_id_1);
        glDeleteTextures(2 , &texture_id_2);
    }

    void UpdateSize(int screenquad_width , int screenquad_height) {
        this->screenquad_width_ = screenquad_width;
        this->screenquad_height_ = screenquad_height;

        int minimum = min(screenquad_width , screenquad_height);
        this->max_kernel = minimum % 2 == 1 ? minimum : minimum - 1;
    }

    void Draw(int pass , float variance) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // window size uniforms
        glUniform1f(glGetUniformLocation(program_id_ , "tex_width") ,
                    this->screenquad_width_);
        glUniform1f(glGetUniformLocation(program_id_ , "tex_height") ,
                    this->screenquad_height_);

        glUniform1i(glGetUniformLocation(program_id_ , "pass") , pass);

        float kernel[max_kernel];

        int radius = 6 * (int) ceil(variance);
        int circumference = 1 + 2 * radius;

        if (circumference > max_kernel) {
            circumference = max_kernel;
            radius = (max_kernel + 1) / 2;
        }

        for (int i = -radius; i <= radius; i++) {
            kernel[i + radius] = exp(-(i * i) / (2.0 * variance * variance * variance));
        }

        glUniform1i(glGetUniformLocation(program_id_ , "radius") , radius);
        glUniform1i(glGetUniformLocation(program_id_ , "circumference") , circumference);
        glUniform1fv(glGetUniformLocation(program_id_ , "kernel") , circumference , kernel);

        if (pass == 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D , texture_id_1);
        }

        if (pass == 1) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D , texture_id_2);
        }

        // draw
        glDrawArrays(GL_TRIANGLE_STRIP , 0 , 4);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
