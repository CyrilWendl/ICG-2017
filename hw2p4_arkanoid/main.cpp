// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "quad/quad.h"

Quad ball;
Quad bar;

float window_width = 800;
float window_height = 600;
float barpos=0.0f;
float time_start;
float deltaTime = 0.0f;
float currentFrame = 0.0f;
float lastFrame = 0.0f;
glm::vec2 pos=glm::vec2(0.0f,-1.0f); // initial position of the ball
float speed=1.0f; // velocity of the ball
bool b_right=true;
bool b_up=true;
bool is_inside=true;
int n_touch=0;

void Init() {
    // sets background color
    glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);

    ball.Init();
    bar.Init();
    time_start=glfwGetTime();
}

void MousePos(GLFWwindow* window, double x, double y) {
    barpos=x/window_width*2.0f-1.0f;
}


void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    float time_s = glfwGetTime()-time_start;
    float increment=time_s/100.0f; //all 10 seconds, double the speed
    const float PI = 3.14159265359;
    glm::mat4 T;
    glm::mat4 R;
    glm::mat4 S;
    /*
     * translations
     * idea: start moving to the right
     *
     *  if pos.x=-1 (move right) else if pos.x=1 (move left)
     *  if pos.y=-1 (move up) else if pos.y=1 (move down)
     * */

    if(is_inside){ //GAME STARTED
        float b=.8;//boundary (pos. and neg.)
        if(pos.x<=-b){
            b_right=true;
        }else if(pos.x>=b){
            b_right=false;
        }

        if(pos.y<=-b){// whenever on bottom, check that bar is there

            if(!(barpos+.3f>pos.x && barpos-.3f<pos.x)){//check that pos between bar max and min
                cout << "GAME OVER :(" << endl;
                cout << "Press R to start a new game" << endl;
                is_inside=false;// GAME OVER :(
            }else {
                is_inside = true;
                // check on which side of the bar it landed and reflect if needed
                if(pos.x<barpos){// ball on left side
                    b_right=false;
                }else{
                    b_right=true;
                }

            }
            n_touch+=1;
            cout<<n_touch<<endl;
            b_up=true;
        }else if(pos.y>=b){
            b_up=false;
        }

        float v=speed*deltaTime+increment/100;//velocity
        if(b_right){
            pos.x+=v;
        }else{
            pos.x-=v;
        }
        if(b_up){
            pos.y+=v;
        }else{
            pos.y-=v;
        }
    }

    T = glm::translate(glm::mat4(1.0f),glm::vec3(pos.x,pos.y,.5));
    float ratio=window_width/window_height;
    S = glm::scale(glm::mat4(1.0f),glm::vec3(.1f/ratio,.1f,.1f));
    glm::mat4 M=T*S;
    ball.Draw(M);
    T = glm::translate(glm::mat4(1.0f),glm::vec3(barpos,-.9,.5));
    ratio=window_width/window_height;
    S = glm::scale(glm::mat4(1.0f),glm::vec3(.4f/ratio,.01f,.1f));
    M=T*S;
    bar.Draw(M);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key ==GLFW_KEY_R){
        is_inside=true;
        time_start=glfwGetTime();
    }
}

void SetupProjection(GLFWwindow *window , int width , int height) {
    window_width = width;
    window_height = height;

    cout << "Window has been resized to "
         << window_width << "x" << window_height << "." << endl;
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
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Arkanoid", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // set the mouse position callback
    glfwSetCursorPosCallback(window, MousePos);

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

    SetupProjection(window , window_width , window_height);

    // render loop
    while(!glfwWindowShouldClose(window)) {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    ball.Cleanup();
    bar.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}

