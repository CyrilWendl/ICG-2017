#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

class Skybox {

private:
    GLuint vertex_array_id_;        // vertex array object
    GLuint program_id_;             // GLSL shader program ID
    GLuint vertex_buffer_object_;   // memory buffer
    GLuint texture_day_id_;         // Day texture ID
    GLuint texture_night_id_;        // Night texture ID
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

            // load night texture
            glGenTextures(1, &texture_night_id_);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_night_id_);

            vector<const GLchar*> faces2;
            faces2.push_back("violentdays_ft.tga");
            faces2.push_back("violentdays_bk.tga");
            faces2.push_back("violentdays_up.tga");
            faces2.push_back("violentdays_dn.tga");
            faces2.push_back("violentdays_rt.tga");
            faces2.push_back("violentdays_lf.tga");

            loadCubemap(faces2);

            GLuint tex_id2 = glGetUniformLocation(program_id_, "skybox_night");
            glUniform1i(tex_id2, 1 /*GL_TEXTURE1*/);

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
        glDeleteTextures(1, &texture_night_id_);
    }

    void Draw(const glm::mat4& MVP) {
        glDepthMask(GL_FALSE);
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_day_id_);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_night_id_);

        // setup MVP
        GLuint MVP_id = glGetUniformLocation(program_id_, "MVP");
        glUniformMatrix4fv(MVP_id, 1, GL_FALSE, value_ptr(MVP));

        // draw
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
        glUseProgram(0);
    }
};
