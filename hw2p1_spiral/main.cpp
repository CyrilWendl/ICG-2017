// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "triangle/triangle.h"

Triangle triangle;

void Init() {
    // sets background color
    glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);

    triangle.Init();
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    const float PI = 3.14159265359;
    glm::mat4 T;
    glm::mat4 R;
    glm::mat4 S;

    bool fermat=true;
    if (!fermat){
        const int N_TRIANGLES = 50;
        const int N_CIRCLES = 3;
        glm::mat4 model[N_TRIANGLES];

        float a = .1;
        float b = .03; // spacing between spiral
        float t; // theta (rotation)
        float r; // radius (distance)
        float s=.08; // scale

        for (int i = 0; i < N_TRIANGLES; i = i + 1) {
            t = (float) i / (float) N_TRIANGLES * (float) N_CIRCLES * (2 * PI);
            r = a + b * t;

            T = glm::translate(glm::mat4(1.0f) ,
                               glm::vec3(r * cos(t), r * sin(t), 0.0f));
            R = glm::rotate(glm::mat4(1.0f) , (t) ,
                            glm::vec3(0.0f , 0.0f , 1.0f));
            S = glm::mat4(1.0f);
            S[0][0] = (float) i / N_TRIANGLES * s;
            S[1][1] = (float) i / N_TRIANGLES * s;
            model[i] = T * S * R;
            triangle.Draw(model[i]);
        }
    }
    else {

        const int N_TRIANGLES = 190;
        glm::mat4 model[N_TRIANGLES];

        float c = .25; // mutliplication factor (distance between circles)
        float t; float r; // theta (angle) and radius
        float s=.02; // scale

        for (int i = 0; i < N_TRIANGLES; i = i + 1) {
            t = (float) i *137.508;
            r = c*sqrt(t);

            T = glm::translate(glm::mat4(1.0f) ,
                               glm::vec3(r * cos(t), r * sin(t), 0.0f));
            R = glm::rotate(glm::mat4(1.0f) , (t) ,
                            glm::vec3(0.0f , 0.0f , 1.0f));
            S = glm::mat4(1.0f);

            // nice feature: add *(float)i/N_TRIANGLES for gradient
            S[0][0] = s*(float)i/N_TRIANGLES;
            S[1][1] = s*(float)i/N_TRIANGLESx;
            model[i] =  S * T * R;
            triangle.Draw(model[i]);

            // in opposite direction
            r = -(c*sqrt(t));
            T = glm::translate(glm::mat4(1.0f) ,
                               glm::vec3(r * cos(t), r * sin(t), 0.0f));
            R = glm::rotate(glm::mat4(1.0f) , (t) ,
                            glm::vec3(0.0f , 0.0f , 1.0f));
            model[i] =  S * T * R;
            triangle.Draw(model[i]);
        }
    }
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, char *argv[]) {
    // GLFW Initialization
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow* window = glfwCreateWindow(512, 512, "Spiral", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init();

    // render loop
    while(!glfwWindowShouldClose(window)) {
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    triangle.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}

