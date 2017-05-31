#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

// Represents a single particle and its state
struct Particle {
    glm::vec2 position, velocity;
    glm::vec4 color;
    GLfloat size, angle, weight;
    GLfloat life;
};

class Particles: public Particle{

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_id_;             // texture ID
        GLuint MVP_id_;                 // Model, view, projection matrix ID
        vector<Particle> particles;     // particles
        GLuint amount = 1000;                  // amount of particles
        GLuint lastUsedParticle = 0;    // index of last used particle

    public:
        void Init() {
            // compile the shaders
            program_id_ = icg_helper::LoadShaders("particles_vshader.glsl",
                                                  "particles_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates
            {
                const GLfloat particle_quad[] = {
                    -0.5f, -0.5f, 0.0f,
                     0.5f, -0.5f, 0.0f,
                     -0.5f, 0.5f, 0.0f,
                     0.5f, 0.5f, 0.0f,
                    };
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad),
                             particle_quad, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "particle_position");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 4, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);

                for (GLuint i = 0; i < this->amount; ++i)
                        this->particles.push_back(Particle());
            }

            // load texture
            {
                int width;
                int height;
                int nb_component;
                string filename = "particle.png";
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
            }

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }

        GLuint firstUnusedParticle()
        {
            // First search from last used particle, this will usually return almost instantly
            for (GLuint i = lastUsedParticle; i < this->amount; ++i){
                if (this->particles[i].life <= 0.0f){
                    lastUsedParticle = i;
                    return i;
                }
            }
            // Otherwise, do a linear search
            for (GLuint i = 0; i < lastUsedParticle; ++i){
                if (this->particles[i].life <= 0.0f){
                    lastUsedParticle = i;
                    return i;
                }
            }
            // All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
            lastUsedParticle = 0;
            return 0;
        }

        void respawnParticle(Particle &particle, glm::vec2 offset)
        {
            GLfloat random = ((rand() % 100) - 50) / 10.0f;
            GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
            particle.position = random + offset;
            particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
            particle.life = 1.0f;
            particle.velocity *= 0.1f;
        }

        void update(GLfloat dt, GLuint newParticles, glm::vec2 offset)
        {
            // Add new particles
            for (GLuint i = 0; i < newParticles; ++i)
            {
                int unusedParticle = this->firstUnusedParticle();
                this->respawnParticle(this->particles[unusedParticle], offset);
            }
            // Update all particles
            for (GLuint i = 0; i < this->amount; ++i)
            {
                Particle &p = this->particles[i];
                p.life -= dt; // reduce life
                if (p.life > 0.0f)
                {	// particle is alive, thus update
                    p.position -= p.velocity * dt;
                    p.color.a -= dt * 2.5;
                }
            }
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteTextures(1, &texture_id_);
        }

        void Draw(const glm::mat4& MVP,
                  const glm::mat4 &projection = IDENTITY_MATRIX,
                  float offset_x = 0.0f, float offset_y = 0.0f) {
            glUseProgram(program_id_);

            // setup MVP
            GLuint MVP_id = glGetUniformLocation(program_id_, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, value_ptr(MVP));

            GLint projection_id = glGetUniformLocation(program_id_,
                                                       "projection");
            glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE,
                               glm::value_ptr(projection));

            // Use additive blending to give it a 'glow' effect
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                for (Particle particle : this->particles)
                {
                    if (particle.life > 0.0f)
                    {
                        glUniform2fv(glGetUniformLocation(program_id_, "offset"), ONE, glm::value_ptr(particle.position));
                        glUniform4fv(glGetUniformLocation(program_id_, "color"), ONE, glm::value_ptr(particle.color));
                        // bind textures
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, texture_id_);
                        glBindVertexArray(vertex_array_id_);
                        glDrawArrays(GL_TRIANGLES, 0, 6);
                        glBindVertexArray(0);
                    }
                }
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glUseProgram(0);
        }
};
