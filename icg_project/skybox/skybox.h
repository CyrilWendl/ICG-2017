#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

class Skybox {

private:
    GLuint vertex_array_id_;        // vertex array object
    GLuint program_id_;             // GLSL shader program ID
    GLuint vertex_buffer_object_;   // memory buffer
    GLuint texture_day_id_;         // Day texture ID
    GLuint texture_sunset_id_;      // Sunset texture ID
    GLuint texture_night_id_;       // Night texture ID
    GLuint MVP_id_;                 // Model, view, projection matrix ID

public:
    void Init() {
        // compile the shaders
        program_id_ = icg_helper::LoadShaders("skybox_vshader.glsl",
                                              "skybox_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex Array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates
        {
            const  GLfloat vertex_point[] = {
                // Positions
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                -1.0f,  1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f,  1.0f
            };
            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point),
                         vertex_point, GL_STATIC_DRAW);

            // attribute
            GLuint vertex_point_id = glGetAttribLocation(program_id_, "sky_position");
            glEnableVertexAttribArray(vertex_point_id);
            glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        // load texture
        {
            //load day texture
            glGenTextures(1, &texture_day_id_);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_day_id_);

            vector<const GLchar*> faces;
            faces.push_back("miramar_ft.tga");
            faces.push_back("miramar_bk.tga");
            faces.push_back("miramar_up.tga");
            faces.push_back("miramar_dn.tga");
            faces.push_back("miramar_rt.tga");
            faces.push_back("miramar_lf.tga");
//            faces.push_back("cottoncandy_rt.tga");
//            faces.push_back("cottoncandy_lf.tga");
//            faces.push_back("cottoncandy_up.tga");
//            faces.push_back("cottoncandy_dn.tga");
//            faces.push_back("cottoncandy_bk.tga");
//            faces.push_back("cottoncandy_ft.tga");
//            faces.push_back("right.jpg");
//            faces.push_back("left.jpg");
//            faces.push_back("top.jpg");
//            faces.push_back("bottom.jpg");
//            faces.push_back("back.jpg");
//            faces.push_back("front.jpg");
//            faces.push_back("cloudtop_rt.tga");
//            faces.push_back("cloudtop_lf.tga");
//            faces.push_back("cloudtop_up.tga");
//            faces.push_back("cloudtop_dn.tga");
//            faces.push_back("cloudtop_bk.tga");
//            faces.push_back("cloudtop_ft.tga");

            loadCubemap(faces);

            // texture uniforms
            GLuint tex_id = glGetUniformLocation(program_id_, "skybox_day");
            glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);

            // cleanup
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            // load sunset texture
            glGenTextures(1, &texture_sunset_id_);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_sunset_id_);

            vector<const GLchar*> faces2;
            faces2.push_back("violentdays_ft.tga");
            faces2.push_back("violentdays_bk.tga");
            faces2.push_back("violentdays_up.tga");
            faces2.push_back("violentdays_dn.tga");
            faces2.push_back("violentdays_rt.tga");
            faces2.push_back("violentdays_lf.tga");

            loadCubemap(faces2);

            GLuint tex_id2 = glGetUniformLocation(program_id_, "skybox_sunset");
            glUniform1i(tex_id2, 1 /*GL_TEXTURE1*/);

            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            // load night texture
            glGenTextures(1, &texture_night_id_);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_night_id_);

            vector<const GLchar*> faces3;
            faces3.push_back("grimmnightft.tga");
            faces3.push_back("grimmnightbk.tga");
            faces3.push_back("grimmnightup.tga");
            faces3.push_back("grimmnightdn.tga");
            faces3.push_back("grimmnightrt.tga");
            faces3.push_back("grimmnightlf.tga");

            loadCubemap(faces3);

            GLuint tex_id3 = glGetUniformLocation(program_id_, "skybox_night");
            glUniform1i(tex_id3, 1 /*GL_TEXTURE2*/);

            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        }

        // other uniforms
        {
            MVP_id_ = glGetUniformLocation(program_id_, "MVP");
        }

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void loadCubemap(vector<const GLchar*> faces)
    {
        int width;
        int height;
        int nb_component;
        string filename;
        //stbi_set_flip_vertically_on_load(1);
        unsigned char* image;
        for(GLuint i = 0; i < faces.size(); i++)
        {
            filename = (string) faces[i];
            image = stbi_load(filename.c_str(), &width,
                              &height, &nb_component, 0);
            if(image == nullptr) {
                throw(string("Failed to load texture"));
            }
            if(nb_component == 3) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0,
                             GL_RGB, GL_UNSIGNED_BYTE, image);
            } else if(nb_component == 4) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0,
                             GL_RGBA, GL_UNSIGNED_BYTE, image);
            }
            stbi_image_free(image);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteTextures(1, &texture_day_id_);
        glDeleteTextures(1, &texture_sunset_id_);
        glDeleteTextures(1, &texture_night_id_);
    }

    void Draw(const glm::mat4& MVP, float time, int daynight_mode, float daynight_pace, int fog_enable, glm::vec3 fog_color) {
        glDepthMask(GL_FALSE);
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // blend factor for day/night cycle
        float blend = 0.0f;

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
                blend = (time_inst - day_start) / (day_end - day_start);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_day_id_);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_day_id_);
            } else if(time_inst >= day_end && time_inst < sunset_start) {
                blend = (time_inst - day_end) / (sunset_start -day_end);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_day_id_);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_sunset_id_);
            } else if(time_inst >= sunset_start && time_inst < sunset_end) {
                blend = (time_inst - sunset_start) / (sunset_end -sunset_start);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_sunset_id_);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_sunset_id_);
            } else if(time_inst >= sunset_end && time_inst < night_start) {
                blend = (time_inst - sunset_end) / (night_start -sunset_end);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_sunset_id_);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_night_id_);
            } else if(time_inst >= night_start && time_inst < night_end) {
                blend = (time_inst - night_start) / (night_end -night_start);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_night_id_);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_night_id_);
            } else {
                blend = (time_inst - night_end) / (next_day -night_end);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_night_id_);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, texture_day_id_);
            }
        } else if(daynight_mode == 1)       // Day
        {
            blend = 0.0f;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_day_id_);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_day_id_);
        } else {                            // Night

            blend = 0.0f;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_night_id_);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_night_id_);

        }

        // setup MVP
        GLuint MVP_id = glGetUniformLocation(program_id_, "MVP");
        glUniformMatrix4fv(MVP_id, 1, GL_FALSE, value_ptr(MVP));

        // pass texture blending factor to the shader.
        glUniform1f(glGetUniformLocation(program_id_, "blend"), blend);

        // pass fog parameters
        glUniform1i(glGetUniformLocation(program_id_, "fog_enable"), fog_enable);
        glUniform3fv(glGetUniformLocation(program_id_, "fog_color"), ONE, glm::value_ptr(fog_color));

        // draw
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
        glUseProgram(0);
    }
};
