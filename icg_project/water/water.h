#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>
#include "../grid/grid.h"

class Water : public Light{

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID
        GLuint texture_id_;                     // texture ID
        GLuint texture_mirror_id_;              // texture mirror ID
        GLuint texture_refract_id_;             // texture refract ID
        GLuint num_indices_;                    // number of vertices to render
        GLuint MVP_id_;                         // model, view, proj matrix ID

    public:
        void Init(GLuint tex_mirror = -1, GLuint tex_refract = -1) {
            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("water_vshader.glsl",
                                                  "water_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates and indices
            {
                std::vector<GLfloat> vertices;
                std::vector<GLuint> indices;
                // always two subsequent entries in 'vertices' form a 2D vertex position.
                int grid_dim = 513;      // number of lateral vertices, make sure it's the same as the grid

                // vertex position of the triangles.
                for (float i = -(grid_dim/2) ; i <= (grid_dim/2) ; ++i)
                {
                    for (float j = -(grid_dim/2) ; j <= (grid_dim/2) ; ++j)
                    {
                        vertices.push_back(float(i)/float(grid_dim/32));
                        vertices.push_back(float(j)/float(grid_dim/32));
                    }
                }


                for (unsigned p = 0; p < (grid_dim*grid_dim) - grid_dim ; ++p)
                {
                    if ((p % grid_dim != grid_dim-1) )      // if we are in border top or right of the floor
                    {
                        indices.push_back(p);
                        indices.push_back(p+grid_dim);
                        indices.push_back(p+grid_dim+1);

                        indices.push_back(p);
                        indices.push_back(p+grid_dim+1);
                        indices.push_back(p+1);
                    }
                }

                num_indices_ = indices.size();

                // position buffer
                glGenBuffers(1, &vertex_buffer_object_position_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                             &vertices[0], GL_STATIC_DRAW);

                // vertex indices
                glGenBuffers(1, &vertex_buffer_object_index_);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                             &indices[0], GL_STATIC_DRAW);

                // position shader attribute
                GLuint loc_position = glGetAttribLocation(program_id_, "position");
                glEnableVertexAttribArray(loc_position);
                glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);

            }

            // load texture
            {
                int width;
                int height;
                int nb_component;
                string filename = "clear_water.jpg";
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

                texture_mirror_id_ = (tex_mirror ==-1)? texture_id_ : tex_mirror;
                texture_refract_id_ = (tex_refract == -1)? texture_id_ : tex_refract;

                // texture uniforms
                GLuint tex_id = glGetUniformLocation(program_id_, "texWater");
                glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);

                // reflection texture uniform
                GLuint tex_mirror_id = glGetUniformLocation(program_id_, "tex_mirror");
                glUniform1i(tex_mirror_id, 1 /*GL_TEXTURE1*/);

                // refraction texture uniform
                GLuint tex_refract_id = glGetUniformLocation(program_id_, "tex_refract");
                glUniform1i(tex_refract_id, 2 /*GL_TEXTURE2*/);

                // cleanup
                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(image);
            }

            // other uniforms
            MVP_id_ = glGetUniformLocation(program_id_, "MVP");

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_position_);
            glDeleteBuffers(1, &vertex_buffer_object_index_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteProgram(program_id_);
            glDeleteTextures(1, &texture_id_);
            glDeleteTextures(1, &texture_mirror_id_);
            glDeleteTextures(1, &texture_refract_id_);
        }

        void Draw(float time, int daynight_mode, float daynight_pace, float water_height,
                  int fog_enable, glm::vec3 fog_color, float fog_density,
                  const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {

            //Setup model Matrix(for light position)
            float scale = 1.0;
            glm::mat4 M = model;
            M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.5f));
            //M = glm::rotate(M, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
            M = glm::scale(M, glm::vec3(scale));

            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_mirror_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, texture_refract_id_);

            // setup MVP
            glm::mat4 MVP = projection*view*model;
            glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

            // pass the current time stamp to the shader.
            glUniform1f(glGetUniformLocation(program_id_, "time"), time);

            Light::Setup(program_id_);

            // Diffuse intensity depending on time of day
            float diffuse_factor = 0.0;
            float diffuse_day = 0.5f;
            float diffuse_sunset = 0.3f;
            float diffuse_night = 0.1;

            // fog blend factor (e.g fog disappears into the sunset)
            float fog_blend = 1.0f;

            // Day/night cycle time frames
            float pace = daynight_pace;   //uniform
            float day_start = 0 * pace;
            float day_end = 1 * pace;
            float sunset_start = 1.5 * pace;
            float sunset_end = 2.5 * pace;
            float night_start = 3 * pace;
            float night_end = 4 * pace;
            int next_day = 4.5 * pace;

            if(daynight_mode == 0)              // Cycle
            {
                // bind appropriate texture for current time
                int time_inst = time * 1000;
                float time_transition = 1.0f;
                time_inst = (int)(time_inst * time_transition) % next_day;
                if(time_inst >= day_start && time_inst < day_end) {

                    diffuse_factor = diffuse_day;

                } else if(time_inst >= day_end && time_inst < sunset_start) {
                    //diffuse_factor transition from 0.4 to 0.3 (Day to sunset)
                    diffuse_factor = diffuse_day + ((diffuse_sunset - diffuse_day) / (sunset_start - day_end)) * (time_inst - day_end);
                    fog_blend = 1.0 - (time_inst - day_end) / (sunset_start - day_end);

                } else if(time_inst >= sunset_start && time_inst < sunset_end) {

                    diffuse_factor = diffuse_sunset;
                    fog_blend = 0.0;

                } else if(time_inst >= sunset_end && time_inst < night_start) {

                    //diffuse_factor transition from 0.3 to 0.1 (Sunset to night)
                    diffuse_factor = diffuse_sunset + ((diffuse_night - diffuse_sunset) / (night_start - sunset_end)) * (time_inst - sunset_end);
                    fog_blend = (time_inst - sunset_end) / (night_start -sunset_end);

                } else if(time_inst >= night_start && time_inst < night_end) {

                    diffuse_factor = diffuse_night;

                } else {
                    //diffuse_factor transition from 0.1 to 0.4 (Night to Day)
                    diffuse_factor = diffuse_night + ((diffuse_day - diffuse_night) / (next_day - night_end)) * (time_inst - night_end);

                }
            } else if(daynight_mode == 1)           // Day
            {
                diffuse_factor = diffuse_day;

            } else {                                // Night

                diffuse_factor = diffuse_night;
            }

            // Pass the diffuse parameter depending on the time of the day
            glUniform1f(glGetUniformLocation(program_id_, "diffuse_factor"), diffuse_factor);

            // Pass the water height parameter as uniform
            glUniform1f(glGetUniformLocation(program_id_, "water_height"), water_height);

            // pass fog parameters
            glUniform1i(glGetUniformLocation(program_id_, "fog_enable"), fog_enable);
            glUniform3fv(glGetUniformLocation(program_id_, "fog_color"), ONE, glm::value_ptr(fog_color));
            glUniform1f(glGetUniformLocation(program_id_, "fog_density"), fog_density);
            //pass fog blending factor to the shader
            glUniform1f(glGetUniformLocation(program_id_, "fog_blend"), fog_blend);

            // setup matrix stack
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

            // draw
            // For debugging it can be helpful to draw only the wireframe.
            // You can do that by uncommenting the next line.
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            // Depending on how you set up your vertex index buffer, you
            // might have to change GL_TRIANGLE_STRIP to GL_TRIANGLES.
            glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
