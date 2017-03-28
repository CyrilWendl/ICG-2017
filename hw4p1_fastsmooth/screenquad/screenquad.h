#pragma once
#include "icg_helper.h"

class ScreenQuad {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_id_;             // texture ID

        float screenquad_width_;
        float screenquad_height_;
        float std_;
        float offsetx_;
        float offsety_;
        GLfloat kernel_[9]= { // MatLab: fspecial('Gaussian',[9 1],1.75) (sigma=1.75)
                0.0169,
                0.0529,
                0.1197,
                0.1954,
                0.2301,
                0.1954,
                0.1197,
                0.0529,
                0.0169
            };

    public:
        void Init(float screenquad_width, float screenquad_height,
                  GLuint texture, float stdev) {

            // set screenquad size
            this->screenquad_width_ = screenquad_width;
            this->screenquad_height_ = screenquad_height;

            // set standard deviation
            this->std_ = 1.0;

            // set (x,y) offset
            this->offsetx_=1.0f/300.0f;
            this->offsety_=0.0f;

            // compile the shaders
            program_id_ = icg_helper::LoadShaders("screenquad_vshader.glsl",
                                                  "screenquad_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates
            {
                const GLfloat vertex_point[] = { /*V1*/ -1.0f, -1.0f, 0.0f,
                                                 /*V2*/ +1.0f, -1.0f, 0.0f,
                                                 /*V3*/ -1.0f, +1.0f, 0.0f,
                                                 /*V4*/ +1.0f, +1.0f, 0.0f};
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point),
                             vertex_point, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // texture coordinates
            {
                const GLfloat vertex_texture_coordinates[] = { /*V1*/ 0.0f, 0.0f,
                                                               /*V2*/ 1.0f, 0.0f,
                                                               /*V3*/ 0.0f, 1.0f,
                                                               /*V4*/ 1.0f, 1.0f};

                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates),
                             vertex_texture_coordinates, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_,
                                                                     "vtexcoord");
                glEnableVertexAttribArray(vertex_texture_coord_id);
                glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE,
                                      ZERO_BUFFER_OFFSET);

            }

            // load/Assign texture
            this->texture_id_ = texture;
            glBindTexture(GL_TEXTURE_2D, texture_id_);
            GLuint tex_id = glGetUniformLocation(program_id_, "tex");
            glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
            glBindTexture(GL_TEXTURE_2D, 0);

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteTextures(1, &texture_id_);
        }

        void UpdateSize(int screenquad_width, int screenquad_height) {
            this->screenquad_width_ = screenquad_width;
            this->screenquad_height_ = screenquad_height;
        }

        float UpdateStd(float stdev){
            this->std_=stdev;
            return this->std_;
        }

        float* UpdateKernel(float kernel[]){
            glUniform1fv(glGetUniformLocation(program_id_, "kernel"), 9 , kernel);
            return this->kernel_;
        }

        void SetTexOffset(float offsetx, float offsety){
            this->offsetx_=offsetx;
            this->offsety_=offsetx;
        }


        void Draw() {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // window size uniforms
            glUniform1f(glGetUniformLocation(program_id_ , "tex_width") ,
                        this->screenquad_width_);
            glUniform1f(glGetUniformLocation(program_id_ , "tex_height") ,
                        this->screenquad_height_);

            glUniform1f(glGetUniformLocation(program_id_ , "std") ,
                        this->std_);

            glUniform2f(glGetUniformLocation(program_id_, "texOffset"),
                        this->offsetx_, this->offsety_);

            glUniform1i(glGetUniformLocation(program_id_, "mode"),
                        1);

            glUniform1fv(glGetUniformLocation(program_id_, "kernel"), 9, this->kernel_);

            // bind texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D , texture_id_);

            // draw
            glDrawArrays(GL_TRIANGLE_STRIP , 0 , 4);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
