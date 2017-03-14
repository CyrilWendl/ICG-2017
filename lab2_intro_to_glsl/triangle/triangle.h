#pragma once
#include "icg_helper.h"

// translate, rotate, scale, perspective
#include <glm/gtc/matrix_transform.hpp>
// value_ptr
#include <glm/gtc/type_ptr.hpp>

class Triangle {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer

    public:
        void Init() {
            // compile the shaders
            program_id_ = icg_helper::LoadShaders("triangle_vshader.glsl",
                                                  "triangle_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // Vertex Buffer
            const GLfloat vertex_point[] = { /*V1*/-1.0f, -1.0f, 0.0f,
                                             /*V2*/ 1.0f, -1.0f, 0.0f,
                                             /*V3*/ 0.0f,  1.0f, 0.0f};

            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point),
                         vertex_point, GL_STATIC_DRAW);

            // vertex point shader attribute
            // fetch Attribute ID for Vertex Positions
            GLuint position = glGetAttribLocation(program_id_, "vpoint");
            glEnableVertexAttribArray(position); // enable it
            glVertexAttribPointer(position, 3, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);

            // setup the uniform
            GLuint M_id = glGetUniformLocation(program_id_, "M");

            // set transformation uniform
            glm::mat4 T = glm::translate(glm::mat4(1.0f),
                                         glm::vec3(0.5f ,0.5f ,0.0f));
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), (float)M_PI/6.0f,
                                      glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 S = glm::mat4(1.0f);
            S[0][0] = 0.25;
            S[1][1] = 0.25;
            glm::mat4 M = T*S*R;
            glUniformMatrix4fv(M_id, 1, GL_FALSE, glm::value_ptr(M));

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
        }
        
        void Draw() {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // draw
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
