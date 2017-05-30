#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

class Tree {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint tex_terrain_id_;                     // texture ID terrain
        GLuint texture_id_;                  // texture ID tree
        GLuint num_indices_;                    // number of vertices to render
        GLuint MVP_id_;                         // Model, view, projection matrix ID
        GLuint height_id_;
        GLfloat pos_x;                          // x position of tree
        GLfloat pos_y;                          // y position of tree

    public:
        void Init(float size, float height, float treePos_x = 0, float treePos_y = 0, GLuint tex_noise = -1) {

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
            float tree_size = size;
            float tree_height = height;

            pos_x = treePos_x;
            pos_y = treePos_y;

            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;

            // vertex coordinates
            {
                // always two subsequent entries in 'vertices' form a 2D vertex position.

                vertices.push_back(-tree_size);
                vertices.push_back(-tree_height);

                vertices.push_back(-tree_size);
                vertices.push_back(tree_height);

                vertices.push_back(tree_size);
                vertices.push_back(-tree_height);

                vertices.push_back(tree_size);
                vertices.push_back(tree_height);

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
                glVertexAttribPointer(loc_position , 2 , GL_FLOAT , DONT_NORMALIZE ,
                                      ZERO_STRIDE , ZERO_BUFFER_OFFSET);
            }

            // load terrain
            {
                tex_terrain_id_ = tex_noise;
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
                glUniform1i(tex_id, 1 /*GL_TEXTURE1*/);

                // cleanup
                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(image);
            }

            // other uniforms
            {
                MVP_id_ = glGetUniformLocation(program_id_, "MVP");
                glUniform1f(glGetUniformLocation(program_id_, "tree_height"), tree_height);
                glUniform1f(glGetUniformLocation(program_id_, "tree_size"), tree_size);
                height_id_ = glGetUniformLocation(program_id_, "height");
                glUniform1f(glGetUniformLocation(program_id_, "treePos_x"), treePos_x);
                glUniform1f(glGetUniformLocation(program_id_, "treePos_y"), treePos_y);
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
            glDeleteTextures(1, &tex_terrain_id_);
            glDeleteTextures(1, &texture_id_);
        }



        void Draw(float time, float offset_x, float offset_y,
                  int fog_enable, glm::vec3 fog_color, float fog_density,
                  const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            // terrain texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex_terrain_id_);

            // tree texture
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

            // pass the current time stamp to the shader.
            glUniform1f(glGetUniformLocation(program_id_, "time"), time);

            // setup matrix stack
            // 3*3 part of model is transpose of the 3*3 part of the view matrix (But translation incorrect)
            //glm::vec2 pos = glm::vec2(pos_x - 8.0 * offset_x, pos_y - 8.0 * offset_y);
            //glm::mat4 M = translate(model, glm::vec3(pos, 1.0));
            glm::mat4 M = model;
            M[0][0] = view[0][0];
            M[0][1] = view[1][0];
            M[0][2] = view[2][0];
            M[1][0] = view[0][1];
            M[1][1] = view[1][1];
            M[1][2] = view[2][1];
            M[2][0] = view[0][2];
            M[2][1] = view[1][2];
            M[2][2] = view[2][2];

            GLint model_id = glGetUniformLocation(program_id_,
                                                  "model");
            glUniformMatrix4fv(model_id, ONE, DONT_TRANSPOSE, glm::value_ptr(M));
            GLint view_id = glGetUniformLocation(program_id_,
                                                 "view");
            glUniformMatrix4fv(view_id, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
            GLint projection_id = glGetUniformLocation(program_id_,
                                                       "projection");
            glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE,
                               glm::value_ptr(projection));
            // pass parameters
            glUniform1f(glGetUniformLocation(program_id_, "offset_x"), offset_x);
            glUniform1f(glGetUniformLocation(program_id_, "offset_y"), offset_y);

            // pass fog parameters
            glUniform1i(glGetUniformLocation(program_id_, "fog_enable"), fog_enable);
            glUniform3fv(glGetUniformLocation(program_id_, "fog_color"), ONE, glm::value_ptr(fog_color));
            glUniform1f(glGetUniformLocation(program_id_, "fog_density"), fog_density);

            // draw
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
