// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "vector"

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "quad/quad.h"

// Quad stuff1;
// ...
Quad Sun;
Quad Earth;
Quad Moon;
glm::mat4 TransE = glm::mat4(1.0f);

void Init() {
    // sets background color
    glClearColor(1.0,1.0,1.0 /*white*/, 1.0 /*solid*/);
    // {stuff}.Init(...);
    Sun.Init("sun.tga");
    Earth.Init("earth.tga");
    Moon.Init("moon.tga");

}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    float time_s = glfwGetTime();

    // compute the transformation matrices
    // --------------- Utiliser gravitation.calculate !
    // {stuff}.Draw({stuff}_modelmatrix);
    glm::mat4 ScaleS = glm::mat4(1.0f);
    ScaleS[0][0]=0.1f;
    ScaleS[1][1]=0.1f;

    glm::mat4 TransS = glm::mat4(1.0f);
    TransS[3][0] = 0.5f;
    TransS[3][1] = 0.0f;

    glm::mat4 RotS = glm::mat4(1.0f);
    RotS[0][0] = cos(0.25*3.1415*time_s);
    RotS[1][1] = cos(0.25*3.1415*time_s);
    RotS[0][1] = sin(0.25*3.1415*time_s);
    RotS[1][0] = -sin(0.25*3.1415*time_s);

    glm::mat4 ScaleE = glm::mat4(1.0f);
    ScaleE[0][0] = 0.05f;
    ScaleE[1][1] = 0.05f;

    glm::mat4 RotE = glm::mat4(1.0f);
    RotE[0][0] = cos(2*time_s);
    RotE[1][0] = -sin(2*time_s);
    RotE[0][1] = sin(2*time_s);
    RotE[1][1] = cos(2*time_s);

    double e = 0.5;     // excentricity of the ellipse
    double omega = 0.5*3.1415;  // "rotation" speed
    double distES = sqrt((TransE[3][0]- TransS[3][0])*(TransE[3][0]- TransS[3][0]) + (TransE[3][1]- TransS[3][1])*(TransE[3][1]- TransS[3][1]));
    double CoefE = 1.0-distES;        // artificial coefficient to apply to the speed, based on the distance between Earth and Sun
    TransE[3][0] = CoefE*cos(omega*time_s);
    TransE[3][1] = CoefE*e*sin(omega*time_s);

    glm::mat4 ScaleM = glm::mat4(1.0f);
    ScaleM[0][0] = 0.025f;
    ScaleM[1][1] = 0.025f;

    glm::mat4 RotM = glm::mat4(1.0f);
    RotM[0][0] = cos(4*time_s);
    RotM[1][0] = -sin(4*time_s);
    RotM[0][1] = sin(4*time_s);
    RotM[1][1] = cos(4*time_s);

    glm::mat4 TransM = glm::mat4(1.0f);
    TransM[3][0] = TransE[3][0] + 0.25*cos(omega*4*time_s) + 0.01;
    TransM[3][1] = TransE[3][1] + 0.25*0.7*sin(omega*4*time_s);
    double distEM = (time_s==0.0 ? 0 :sqrt((TransM[3][0]- TransE[3][0])*(TransM[3][0]- TransE[3][0]) + (TransM[3][1]- TransE[3][1])*(TransM[3][1]- TransE[3][1])));
    double CoefM = 1-distEM;
    TransM[3][0] = TransM[3][0]+0.05*CoefM *cos(omega*4*time_s);
    TransM[3][1] = TransM[3][1]+0.05*CoefM*0.7*sin(omega*4*time_s);

    Sun.Draw(RotS,ScaleS, TransS);
    Earth.Draw(RotE,ScaleE,TransE);
    Moon.Draw(RotM, ScaleM,TransM);
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
    GLFWwindow* window = glfwCreateWindow(512, 512, "planets", NULL, NULL);
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

    // {stuff}.Cleanup()
    Sun.Cleanup();
    Earth.Cleanup();
    Moon.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}