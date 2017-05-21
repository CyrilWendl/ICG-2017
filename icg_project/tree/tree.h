#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

class Tree {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_id_;                     // texture ID
        GLuint texture_grass_;                  // grass texture
        GLuint texture_rock_;                   // rock texture
        GLuint num_indices_;                    // number of vertices to render
        GLuint MVP_id_;                         // Model, view, projection matrix ID

    public:
        void Init(GLuint tex_noise = -1) {
            // compile the shaders
            program_id_ = icg_helper::LoadShaders("tree_vshader.glsl",
                                                  "tree_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);
            float tree_size = 0.02;
            float tree_height = .5f;

            // vertex coordinates
            {
                std::vector<GLfloat> vertices;
                std::vector<GLuint> indices;
                // always two subsequent entries in 'vertices' form a 2D vertex position.


//                vertices.push_back(-tree_size);       // bottom left vertex
//                vertices.push_back(-tree_size);
//                vertices.push_back(0);

//                vertices.push_back(-tree_size);       // top left vertex
//                vertices.push_back(tree_size);
//                vertices.push_back(0);

//                vertices.push_back(0);        // tree top vertex
//                vertices.push_back(tree_size/float(2.0));
//                vertices.push_back(tree_height);

//                vertices.push_back(tree_size);        // top right vertex
//                vertices.push_back(tree_size);
//                vertices.push_back(0);

//                vertices.push_back(tree_size);        // bottom right vertex
//                vertices.push_back(-tree_size);
//                vertices.push_back(0);

//                // left triangle
//                indices.push_back(0);
//                indices.push_back(2);
//                indices.push_back(1);
//                // top triangle
//                indices.push_back(1);
//                indices.push_back(2);
//                indices.push_back(3);
//                // right triangle
//                indices.push_back(3);
//                indices.push_back(2);
//                indices.push_back(4);
//                // bottom triangle
//                indices.push_back(4);
//                indices.push_back(2);
//                indices.push_back(0);

                vertices.push_back(-tree_size);
                vertices.push_back(-tree_size);

                vertices.push_back(-tree_size);
                vertices.push_back(tree_size);

                vertices.push_back(tree_size);
                vertices.push_back(-tree_size);

                vertices.push_back(tree_size);
                vertices.push_back(tree_size);

                indices.push_back(0);
                indices.push_back(1);
                indices.push_back(2);
                indices.push_back(3);

                num_indices_ = indices.size();

                // position buffer
                glGenBuffers(1 , &vertex_buffer_object_position_);
                glBindBuffer(GL_ARRAY_BUFFER , vertex_buffer_object_position_);
                glBufferData(GL_ARRAY_BUFFER , vertices.size() * sizeof(GLfloat) ,
                             &vertices[0] , GL_STATIC_DRAW);

                // vertex indices
                glGenBuffers(1 , &vertex_buffer_object_index_);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , vertex_buffer_object_index_);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER , indices.size() * sizeof(GLuint) ,
                             &indices[0] , GL_STATIC_DRAW);

                // position shader attribute
                GLuint loc_position = glGetAttribLocation(program_id_ , "position");
                glEnableVertexAttribArray(loc_position);
                glVertexAttribPointer(loc_position , 2/*3*/ , GL_FLOAT , DONT_NORMALIZE ,
                                      ZERO_STRIDE , ZERO_BUFFER_OFFSET);
            }

            // load terrain
            {
                texture_id_ = tex_noise;
                // texture uniforms
                GLuint tex_id = glGetUniformLocation(program_id_ , "texNoise");
                glUniform1i(tex_id , 0 /*GL_TEXTURE0*/);

                // cleanup
                glBindTexture(GL_TEXTURE_2D , 0);
            }

            // load texture
            {
                int width;
                int height;
                int nb_component;
                string filename = "tree_texture.tga";
                // set stb_image to have the same coordinates as OpenGL
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image = stbi_load(filename.c_str(), &width,
                                                 &height, &nb_component, 0);

                if(image == nullptr) {
                    throw(string("Failed to load texture"));
                }

                glGenTextures(1, &texture_id_);
                glBindTexture(GL_TEXTURE_2D, texture_id_);
                glGenerateMipmap(GL_TEXTURE_2D);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image);
                }

                // texture uniforms
                GLuint tex_id = glGetUniformLocation(program_id_, "tex");
                glUseProgram(program_id_);
                glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);

                // cleanup
                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(image);
            }

            // other uniforms
            {
                MVP_id_ = glGetUniformLocation(program_id_, "MVP");
                glUniform1f(glGetUniformLocation(program_id_, "tree_height"), tree_height);
            }

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

        void Draw(float time, const glm::mat4& MVP, float offset_x, float offset_y, float persistance=0.0f) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

            // pass the current time stamp to the shader.
            glUniform1f(glGetUniformLocation(program_id_, "time"), time);

            // setup MVP
            GLuint MVP_id = glGetUniformLocation(program_id_, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, value_ptr(MVP));

            // pass parameters
            glUniform1f(glGetUniformLocation(program_id_, "offset_x"), offset_x);
            glUniform1f(glGetUniformLocation(program_id_, "offset_y"), offset_y);
            glUniform1f(glGetUniformLocation(program_id_, "persistance"), persistance);

            // draw
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
