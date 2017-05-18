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
            float tree_pos = 0.05;
            float tree_height = -30.0;

            // vertex coordinates
            {
                std::vector<GLfloat> vertices;
                std::vector<GLuint> indices;
                // always two subsequent entries in 'vertices' form a 2D vertex position.


                vertices.push_back(-tree_pos);       // bottom left vertex
                vertices.push_back(-tree_pos);
                vertices.push_back(0);

                vertices.push_back(-tree_pos);       // top left vertex
                vertices.push_back(tree_pos);
                vertices.push_back(0);

                vertices.push_back(0);        // tree top vertex
                vertices.push_back(tree_pos/float(2.0));
                vertices.push_back(tree_height);

                vertices.push_back(tree_pos);        // top right vertex
                vertices.push_back(tree_pos);
                vertices.push_back(0);

                vertices.push_back(tree_pos);        // bottom right vertex
                vertices.push_back(-tree_pos);
                vertices.push_back(0);

                // left triangle
                indices.push_back(0);
                indices.push_back(1);
                indices.push_back(2);
                // top triangle
                indices.push_back(1);
                indices.push_back(3);
                indices.push_back(2);
                // right triangle
                indices.push_back(3);
                indices.push_back(4);
                indices.push_back(2);
                // bottom triangle
                indices.push_back(4);
                indices.push_back(0);
                indices.push_back(2);

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
                glVertexAttribPointer(loc_position , 3 , GL_FLOAT , DONT_NORMALIZE ,
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

        void Draw(const glm::mat4& MVP, float offset_x, float offset_y, float persistance=0.0f) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

            // setup MVP
            GLuint MVP_id = glGetUniformLocation(program_id_, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, value_ptr(MVP));

            // pass parameters
            glUniform1f(glGetUniformLocation(program_id_, "offset_x"), offset_x);
            glUniform1f(glGetUniformLocation(program_id_, "offset_y"), offset_y);
            glUniform1f(glGetUniformLocation(program_id_, "persistance"), persistance);

            //TODO make random
            int Perm[256] = { 151,160,137,91,90,15,
                              131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
                              190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
                              88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
                              77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
                              102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
                              135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
                              5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
                              223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
                              129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
                              251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
                              49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
                              138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
            };
            glUniform1iv(glGetUniformLocation(program_id_, "Perm"), 256, Perm);


            // draw
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
