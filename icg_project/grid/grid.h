#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

struct Light {
    glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 light_pos = glm::vec3(0.0f, 0.0f, 2.0f);

    // pass light properties to the shader
    void Setup(GLuint program_id) {
        glUseProgram(program_id);

        // given in camera space
        GLuint light_pos_id = glGetUniformLocation(program_id, "light_pos");

        GLuint Ld_id = glGetUniformLocation(program_id, "Ld");

        glUniform3fv(light_pos_id, ONE, glm::value_ptr(light_pos));
        glUniform3fv(Ld_id, ONE, glm::value_ptr(Ld));
    }
};

class Grid : public Light {

private:
    GLuint vertex_array_id_;                // vertex array object
    GLuint vertex_buffer_object_position_;  // memory buffer for positions
    GLuint vertex_buffer_object_index_;     // memory buffer for indices
    GLuint program_id_;                     // GLSL shader program ID
    GLuint texture_id_;                     // texture ID
    GLuint texture_grass_;                  // grass texture
    GLuint texture_rock_;                   // rock texture
    GLuint texture_snow_;                   // snow texture
    GLuint texture_sand_;                   // sand texture
    GLuint texture_ocean_;                  // ocean texture
    GLuint num_indices_;                    // number of vertices to render
    GLuint MVP_id_;                         // model, view, proj matrix ID

public:
    void Init(GLuint tex_noise = -1) {
        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("grid_vshader.glsl" ,
                                              "grid_fshader.glsl");
        if (!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex array
        glGenVertexArrays(1 , &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates and indices
        {
            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;
            // always two subsequent entries in 'vertices' form a 2D vertex position.
            int grid_dim = 1024+1;      // number of lateral vertices                              CHANGE HERE DIMENSION OF GRID

            // vertex position of the triangles.
            // TODO consider replacing with native function
            for (float i = -(grid_dim / 2); i <= (grid_dim / 2); ++i) {
                for (float j = -(grid_dim / 2); j <= (grid_dim / 2); ++j) {
                    vertices.push_back(float(i) / float(grid_dim / 32));// grid going from -16 to +16
                    vertices.push_back(float(j) / float(grid_dim / 32));
                }
            }


            for (unsigned p = 0; p < (grid_dim * grid_dim) - grid_dim; ++p) {
                if ((p % grid_dim != grid_dim - 1))      // if we are in border top or right of the floor
                {
                    indices.push_back(p);
                    indices.push_back(p + grid_dim);
                    indices.push_back(p + grid_dim + 1);

                    indices.push_back(p);
                    indices.push_back(p + grid_dim + 1);
                    indices.push_back(p + 1);
                }
            }

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

            //TODO est-ce qu'on peut enlever ça?
//                // texture shader
//                GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_,
//                                                                     "vtexcoord");
//                glEnableVertexAttribArray(vertex_texture_coord_id);
//                glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
//                                      DONT_NORMALIZE, ZERO_STRIDE,
//                                      ZERO_BUFFER_OFFSET);
        }

        // load texture
        {
            texture_id_ = tex_noise;
            // texture uniforms
            GLuint tex_id = glGetUniformLocation(program_id_ , "texNoise");
            glUniform1i(tex_id , 0 /*GL_TEXTURE0*/);

            // cleanup
            glBindTexture(GL_TEXTURE_2D , 0);
        }
        // load textures
        {
            //grass texture
            int width;
            int height;
            int nb_component;
            string filename = "grass.tga";
            // set stb_image to have the same coordinates as OpenGL
            // stbi_set_flip_vertically_on_load(1);
            unsigned char *image = stbi_load(filename.c_str() , &width ,
                                             &height , &nb_component , 0);

            if (image == nullptr) {
                throw (string("Failed to load texture"));
            }

            glGenTextures(1 , &texture_grass_);
            glBindTexture(GL_TEXTURE_2D , texture_grass_);
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR);

            if (nb_component == 3) {
                glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGB , width , height , 0 ,
                             GL_RGB , GL_UNSIGNED_BYTE , image);
            } else if (nb_component == 4) {
                glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGBA , width , height , 0 ,
                             GL_RGBA , GL_UNSIGNED_BYTE , image);
            }

            // texture uniforms
            GLuint tex_id = glGetUniformLocation(program_id_ , "tex_grass");
            glUniform1i(tex_id , 1 /*GL_TEXTURE1*/);

            // cleanup
            glBindTexture(GL_TEXTURE_2D , 0);
            stbi_image_free(image);

            // rock texture
            string filename2= "rock.tga";
            // set stb_image to have the same coordinates as OpenGL
            // stbi_set_flip_vertically_on_load(1);
            unsigned char *image2 = stbi_load(filename2.c_str() , &width ,
                                              &height , &nb_component , 0);

            if (image2 == nullptr) {
                throw (string("Failed to load texture"));
            }

            glGenTextures(1 , &texture_rock_);
            glBindTexture(GL_TEXTURE_2D , texture_rock_);
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR);

            if (nb_component == 3) {
                glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGB , width , height , 0 ,
                             GL_RGB , GL_UNSIGNED_BYTE , image2);
            } else if (nb_component == 4) {
                glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGBA , width , height , 0 ,
                             GL_RGBA , GL_UNSIGNED_BYTE , image2);
            }

            // texture uniforms
            GLuint tex_rock_id = glGetUniformLocation(program_id_ , "tex_rock");
            glUniform1i(tex_rock_id , 2 ); //GL_TEXTURE2

            // cleanup
            glBindTexture(GL_TEXTURE_2D , 0);
            stbi_image_free(image2);

            // snow texture
            string filename3= "snow.tga";
            // set stb_image to have the same coordinates as OpenGL
            // stbi_set_flip_vertically_on_load(1);
            unsigned char *image3 = stbi_load(filename3.c_str() , &width ,
                                              &height , &nb_component , 0);

            if (image3 == nullptr) {
                throw (string("Failed to load texture"));
            }

            glGenTextures(1 , &texture_snow_);
            glBindTexture(GL_TEXTURE_2D , texture_snow_);
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR);

            if (nb_component == 3) {
                glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGB , width , height , 0 ,
                             GL_RGB , GL_UNSIGNED_BYTE , image3);
            } else if (nb_component == 4) {
                glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGBA , width , height , 0 ,
                             GL_RGBA , GL_UNSIGNED_BYTE , image3);
            }

            // texture uniforms
            GLuint tex_snow_id = glGetUniformLocation(program_id_ , "tex_snow");
            glUniform1i(tex_snow_id , 3 ); //GL_TEXTURE3

            // cleanup
            glBindTexture(GL_TEXTURE_2D , 0);
            stbi_image_free(image3);

            // sand texture
            string filename4= "sand.tga";
            // set stb_image to have the same coordinates as OpenGL
            // stbi_set_flip_vertically_on_load(1);
            unsigned char *image4= stbi_load(filename4.c_str() , &width ,
                                             &height , &nb_component , 0);

            if (image4 == nullptr) {
                throw (string("Failed to load texture"));
            }

            glGenTextures(1 , &texture_sand_);
            glBindTexture(GL_TEXTURE_2D , texture_sand_);
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR);

            if (nb_component == 3) {
                glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGB , width , height , 0 ,
                             GL_RGB , GL_UNSIGNED_BYTE , image4);
            } else if (nb_component == 4) {
                glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGBA , width , height , 0 ,
                             GL_RGBA , GL_UNSIGNED_BYTE , image4);
            }

            // texture uniforms
            GLuint tex_sand_id = glGetUniformLocation(program_id_ , "tex_sand");
            glUniform1i(tex_sand_id , 4 ); //GL_TEXTURE4

            // cleanup
            glBindTexture(GL_TEXTURE_2D , 0);
            stbi_image_free(image4);

            // ocean texture
            string filename5= "Oceanfloor.jpg";
            // set stb_image to have the same coordinates as OpenGL
            // stbi_set_flip_vertically_on_load(1);
            unsigned char *image5= stbi_load(filename5.c_str() , &width ,
                                             &height , &nb_component , 0);

            if (image5 == nullptr) {
                throw (string("Failed to load texture"));
            }

            glGenTextures(1 , &texture_ocean_);
            glBindTexture(GL_TEXTURE_2D , texture_ocean_);
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR);

            if (nb_component == 3) {
                glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGB , width , height , 0 ,
                             GL_RGB , GL_UNSIGNED_BYTE , image5);
            } else if (nb_component == 4) {
                glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGBA , width , height , 0 ,
                             GL_RGBA , GL_UNSIGNED_BYTE , image5);
            }

            // texture uniforms
            GLuint tex_ocean_id = glGetUniformLocation(program_id_ , "tex_ocean");
            glUniform1i(tex_ocean_id , 5 ); //GL_TEXTURE5

            // cleanup
            glBindTexture(GL_TEXTURE_2D , 0);
            stbi_image_free(image5);
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
        glDeleteTextures(1, &texture_rock_);
        glDeleteTextures(1, &texture_grass_);
        glDeleteTextures(1, &texture_sand_);
        glDeleteTextures(1, &texture_snow_);
        glDeleteTextures(1, &texture_ocean_);
    }

    void Draw(float time, int daynight_mode, float daynight_pace, float water_height,
              const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX, float offset_x=0.0f, float offset_y=0.0f,
              int fog_enable = 0, glm::vec3 fog_color = glm::vec3(0.0), float fog_density = 0.0f,
              float reflect_clipped = 0.0f, float refract_clipped = 0.0f) {

        //Setup model Matrix(for light position)
        float scale = 1.0;
        glm::mat4 M = model;
        M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.5f));
        // movement
        //M = glm::rotate(M, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        M = glm::scale(M, glm::vec3(scale));

        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_grass_);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture_rock_);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture_snow_);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture_sand_);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture_ocean_);

        // setup MVP
        glm::mat4 MVP = projection*view*model;
        glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

        // pass the current time stamp to the shader.
        glUniform1f(glGetUniformLocation(program_id_, "time"), time);

        glUniform1f(glGetUniformLocation(program_id_, "offset_x"), offset_x);
        glUniform1f(glGetUniformLocation(program_id_, "offset_y"), offset_y);

        Light::Setup(program_id_);

        // Diffuse intensity depending on time of day
        float diffuse_factor = 0.0;
        float diffuse_day = 0.4f;
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


        // Pass the water height parameter as uniform
        glUniform1f(glGetUniformLocation(program_id_, "water_height"), water_height);

        // Pass the diffuse parameter depending on the time of the day
        glUniform1f(glGetUniformLocation(program_id_, "diffuse_factor"), diffuse_factor);

        // Used for clipping the terrain (When drawing reflection)
        glUniform1f(glGetUniformLocation(program_id_, "reflect_clipped"), reflect_clipped);

        // Used for clipping the terrain (When drawing refraction)
        glUniform1f(glGetUniformLocation(program_id_, "refract_clipped"), refract_clipped);

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
