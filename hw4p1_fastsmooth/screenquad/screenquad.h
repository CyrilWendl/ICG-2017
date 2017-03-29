#pragma once
#include "icg_helper.h"

class ScreenQuad {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_id_1_;             // texture ID (x)
        GLuint texture_id_2_;             // texture ID (y)

        float screenquad_width_;
        float screenquad_height_;
        float std_;
        float offsetx_;
        float offsety_;

    public:
        void Init(float screenquad_width, float screenquad_height,
                  GLuint texture_1, GLuint texture_2, float std) {

            // set screenquad size
            this->screenquad_width_ = screenquad_width;
            this->screenquad_height_ = screenquad_height;

            // set standard deviation
            this->std_ = std;


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

            // load/Assign textures
            this->texture_id_1_ = texture_1;
            glBindTexture(GL_TEXTURE_2D, texture_id_1_);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glUniform1i(glGetUniformLocation(program_id_, "tex"), 0 /*GL_TEXTURE0*/);

            this->texture_id_2_ = texture_2;
            glBindTexture(GL_TEXTURE_2D, texture_id_2_);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glUniform1i(glGetUniformLocation(program_id_, "tex1"), 1 /*GL_TEXTURE1*/);

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
            glDeleteTextures(1, &texture_id_1_);
        }

        void UpdateSize(int screenquad_width, int screenquad_height) {
            this->screenquad_width_ = screenquad_width;
            this->screenquad_height_ = screenquad_height;
        }

        float UpdateStd(float stdev){
            this->std_=stdev;
            return this->std_;
        }

        void SetTexOffset(float offsetx, float offsety){
            this->offsetx_=offsetx;
            this->offsety_=offsetx;
        }


        void Draw(int pass) {
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

            glUniform1i(glGetUniformLocation(program_id_, "pass"),
                        pass);

            // kernel
            int size = 1 + 3 * 2 * int(ceil(this->std_));
            float kernel[2*size+1];
            for (int i = -size; i <= size; i++){
                kernel[i] = exp(-(i*i)/(2.0*this->std_*this->std_*this->std_*this->std_));
            }

            glUniform1i(glGetUniformLocation(program_id_, "size"),
                        size);

            glUniform1fv(glGetUniformLocation(program_id_, "kernel"),
                         2*size+1,kernel);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D , texture_id_1_);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D , texture_id_2_);


            // draw
            glDrawArrays(GL_TRIANGLE_STRIP , 0 , 4);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
