// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include "grid/grid.h"
#include "quad/quad.h"
#include "water/water.h"
#include "skybox/skybox.h"
#include "framebuffer.h"
#include "tree/tree.h"

#define CAM_DEFAULT "Camera: Default"
#define CAM_FPS "Camera: FPS"
#define TEX_HEIGHT 1024
#define TEX_WIDTH 1024
#define TEX_BITS 1

int window_width = 800;
int window_height = 600;

using namespace glm;

mat4 projection_matrix;
mat4 view_matrix;
mat4 quad_model_matrix;

// Camera
glm::vec3 cameraPos = vec3(0.0f , 1.0f , 0.0f);
glm::vec3 cam_pos_mirr = vec3(cameraPos.x, -cameraPos.y, cameraPos.z);
glm::vec3 cameraFront = vec3(0.0f , -.3f , -.7f);
glm::vec3 cameraUp = vec3(0.0f , 1.0f , 0.0f);
GLfloat yaw_cam = -90.0f;    // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch_cam = -90.0f*(3.0f/10.0f);
double lastX = window_width / 2.0;
double lastY = window_height / 2.0;
GLfloat fov = 45.0f;
bool keys[1024];
string cameraMode=CAM_DEFAULT;

// Delta time
float deltaTime = 0.0f;    // Time between current frame and last frame
float lastFrame = 0.0f;    // Time of last frame
float releaseTime = 0.0f;
float pressTime = 0.0f;
float pressedTime = 0.0f;
float timeDiff = 200.0f;
int mode=0;
char lastkey = 'X';

FrameBuffer framebuffer;
FrameBuffer reflection_buffer;
Grid grid;
Quad quad;
Water water;
Skybox skybox;
Tree tree;

//skybox rotation scale
float sky_rspeed = 0.02;

GLfloat tex[TEX_BITS]; // window height * window width * floats per pixel
float oldHeight;

int octaves = 4;
float amplitude = .7f;
float frequency = 0.5f;
float H = 1;
float lacunarity = 2.5f;
float persistance = 3.5f;

vec3 offset = vec3(0.0f);

mat4 PerspectiveProjection(float left , float right , float bottom ,
                           float top , float near , float far) {
    assert(right > left);
    assert(-far > -near);
    assert(top > bottom);
    mat4 projection = mat4(1.0f);

    projection[0][0] = 2.0f * near / (right - left);
    projection[1][1] = 2.0f * near / (top - bottom);
    projection[2][0] = (right + left) / (right - left);
    projection[2][1] = (top + bottom) / (top - bottom);
    projection[2][2] = -(far + near) / (far - near);
    projection[2][3] = -1.0f;
    projection[3][2] = -(2.0f * far * near) / (far - near);
    projection[3][3] = 0.0f;
    return projection;
}

// transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 TransformScreenCoords(GLFWwindow *window , int x , int y) {
    // the framebuffer and the window doesn't necessarily have the same size
    // i.e. hidpi screens. so we need to get the correct one
    int width;
    int height;
    glfwGetWindowSize(window , &width , &height);
    return vec2(2.0f * (float) x / width - 1.0f ,
                1.0f - 2.0f * (float) y / height);
}


// Gets called when the windows/framebuffer is resized.
void SetupProjection(GLFWwindow *window , int width , int height) {
    window_width = width;
    window_height = height;

    cout << "Window has been resized to "
         << window_width << "x" << window_height << "." << endl;

    glViewport(0 , 0 , window_width/2 , window_height/2);

    GLfloat top = 0.2f;
    GLfloat right = (GLfloat) window_width / window_height * top;
    projection_matrix = PerspectiveProjection(-right , right , -top , top , 0.25f , -0.25f);

    reflection_buffer.Cleanup();
    reflection_buffer.Init(window_width, window_height);
}

void ErrorCallback(int error , const char *description) {
    fputs(description , stderr);
}

void Init(GLFWwindow *window) {
    // sets background color
    glClearColor(0.937 , 0.937 , 0.937 /*gray*/, 1.0 /*solid*/);


    // enable depth test.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    //enable plane clipping
    glEnable(GL_CLIP_DISTANCE0);
    //glEnable(GL_BLEND); // for transparency
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // initialize framebuffer
    glfwGetFramebufferSize(window , &window_width , &window_height);
    GLuint framebuffer_texture_id = framebuffer.Init(TEX_WIDTH , TEX_HEIGHT);
    GLuint reflection_buffer_texid = reflection_buffer.Init(window_width, window_height);
    // initialize the quad with the framebuffer calculated perlin noise texture
    grid.Init(framebuffer_texture_id);
    skybox.Init();
    water.Init(reflection_buffer_texid);
    quad.Init();
    tree.Init(.2f,.01f,framebuffer_texture_id);     // make the tree height random from 0.1 to 0.5

}

// gets called for every frame.
void Display() {
    glViewport(0 , 0 , window_width , window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float time = (float) glfwGetTime();

    // view matrix after removing the translated component
    glm::mat4 view = glm::mat4(glm::mat3(view_matrix));
    //skybox rotation
    view = glm::rotate(view, time * sky_rspeed, glm::vec3(0.0f, 1.0f, 0.0f));

    //view matrix from inverted camera position
    mat4 view_mirr = lookAt(cam_pos_mirr, vec3(cameraFront.x, -cameraFront.y, cameraFront.z), cameraUp);
    mat4 view_projection_mirr = projection_matrix * view_mirr ;

    // mirrored view matrix after removing the translated component
    glm::mat4 sky_mirrview = glm::mat4(glm::mat3(view_mirr));
    //reflected skybox rotation
    sky_mirrview = glm::rotate(view, time * sky_rspeed, glm::vec3(0.0f, 1.0f, 0.0f));

    framebuffer.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        quad.Draw(projection_matrix * view_matrix * quad_model_matrix , octaves , amplitude ,
                  frequency,H,lacunarity, offset.x, offset.z,persistance);
    }
    /*GLfloat *size;
    glGetTextureLevelParameterfv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,size);
    cout << "Size: " << size << endl;*/

    glReadPixels(TEX_WIDTH/2,TEX_HEIGHT/2,1,1, GL_RED, GL_FLOAT, tex);
    // glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, tex);
    framebuffer.Unbind();

    reflection_buffer.Bind();
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox.Draw(projection_matrix * sky_mirrview * quad_model_matrix, time);
        //grid.Draw(time , quad_model_matrix , sky_mirrview , projection_matrix, offset.x, offset.z);
    }
    reflection_buffer.Unbind();

    glViewport(0 , 0 , window_width , window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    skybox.Draw(projection_matrix * view * quad_model_matrix, time);
    grid.Draw(time , quad_model_matrix , view_matrix , projection_matrix, offset.x, offset.z);
    water.Draw(time , quad_model_matrix , view_matrix , projection_matrix);
    mat4 FacingTransfo = projection_matrix * view_matrix * quad_model_matrix;
    FacingTransfo[0][0] = 1.0f;
    FacingTransfo[1][1] = 1.0f;
    FacingTransfo[2][2] = 1.0f;
    FacingTransfo[0][1] = 0.0f;
    FacingTransfo[0][2] = 0.0f;
    FacingTransfo[1][0] = 0.0f;
    FacingTransfo[1][2] = 0.0f;
    FacingTransfo[2][0] = 0.0f;
    FacingTransfo[2][1] = 0.0f;

    tree.Draw(time, projection_matrix * view_matrix * quad_model_matrix, offset.x, offset.z);

}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow *window , int key , int scancode , int action , int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window , GL_TRUE);
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressTime = glfwGetTime(); // measure time between key is pressed and released for slow movement
            keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            releaseTime = glfwGetTime();
            float speed=2.0; // inertia parameter
            pressedTime = (releaseTime-pressTime)*speed;
            if(pressedTime>3.0f)
                pressedTime=3.0f;
            keys[key] = false;
        }
    }

// Terrain
    if (keys[GLFW_KEY_1]){
        octaves += 1;
        cout << "octaves: " << octaves << endl;
    }
    if (keys[GLFW_KEY_2]){
        octaves -= 1;
        cout << "octaves: " << octaves << endl;
    }
    if (keys[GLFW_KEY_3]){
        H += .1;
        cout << "H: " << H << endl;
    }
    if (keys[GLFW_KEY_4]){
        H -= .1;
        cout << "H: " << H << endl;
    }
    if (keys[GLFW_KEY_5]) {
        lacunarity += .5;
        cout << "Lacunarity: " << lacunarity << endl;
    }
    if (keys[GLFW_KEY_6]){
        lacunarity -= .5;
        cout << "Lacunarity: " << lacunarity << endl;
    }
    if (keys[GLFW_KEY_7])
        amplitude += .1;
    if (keys[GLFW_KEY_8])
        amplitude -= .1;
    if (keys[GLFW_KEY_9])
        persistance += .1;
    if (keys[GLFW_KEY_0])
        persistance -= .1;
    if (keys[GLFW_KEY_RIGHT])
        frequency += .1;
    if (keys[GLFW_KEY_LEFT])
        frequency -= .1;
    if (keys[GLFW_KEY_H]){//help
        cout << "parameters:\n1 - octave+=.1\n2 - octave-=.1\n3 - H+=.1\n4 - H-=.1\n5 - lacunarity-=.5\n6 - lacunarity+=.5\n7 - amplitude+=.1\n8 - amplitude-=.1\n9 - persistance +=.1\n0 - persistance-=.1\nright - frequency+=.1\nleft - frequency -=.1"<<endl;
    }
    if (keys[GLFW_KEY_X]){// debug
        cout << "cameraPos:   " << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << endl;
        cout << "cameraFront: " << cameraFront.x << ", " << cameraFront.y << ", " << cameraFront.z << endl;
        cout << "cameraUp:    " << cameraUp.x << ", " << cameraUp.y << ", " << cameraUp.z << endl;
        cout << "offset x:    " << offset.x << endl;
        cout << "offset y:    " << offset.y << endl;
    }
    if (keys[GLFW_KEY_F]){
        if(cameraMode==CAM_DEFAULT){
            cameraMode=CAM_FPS;
        } else {
            cameraMode=CAM_DEFAULT;
        }
        cout << cameraMode << endl;
    }
}

void check_pitch(){
    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch_cam > 89.0f)
        pitch_cam = 89.0f;
    if (pitch_cam < -89.0f)
        pitch_cam = -89.0f;
}

void move_terrain() {
    timeDiff = (glfwGetTime()-releaseTime);

    float intensity = (pressedTime - timeDiff)/pressedTime;

    if (intensity<0)
        lastkey='X';

    // Camera controls
    GLfloat cameraSpeed = .2f * deltaTime;
    if (keys[GLFW_KEY_W] || (timeDiff < pressedTime && lastkey=='W')){// move on terrain
        lastkey='W';
        if (timeDiff>0 && intensity>0)
            cameraSpeed *= intensity;
        offset += cameraSpeed * cameraFront;
    }
    if (keys[GLFW_KEY_S] || (timeDiff < pressedTime && lastkey=='S')){
        lastkey='S';
        if (timeDiff>0 && intensity>0)
            cameraSpeed *= intensity;
        offset -= cameraSpeed * cameraFront;
    }
    if (keys[GLFW_KEY_R] || (timeDiff < pressedTime && lastkey=='R')){
        lastkey='R';
        if (timeDiff>0 && intensity>0)
            cameraSpeed *= intensity;
        cameraPos.y -= 10*cameraSpeed;
    }
    if (keys[GLFW_KEY_T] || (timeDiff < pressedTime && lastkey=='T')){
        lastkey='T';
        if (timeDiff>0 && intensity>0)
            cameraSpeed *= intensity;
        cameraPos.y += 10*cameraSpeed;;
    }
    if (keys[GLFW_KEY_A] || (timeDiff < pressedTime && lastkey=='A')){
        lastkey='A';
        //cameraPos -= glm::normalize(glm::cross(cameraFront , cameraUp)) * cameraSpeed
        GLfloat xoffset = cameraSpeed* 30;    // Change this value to your liking
        if (timeDiff>0 && intensity>0)
            xoffset *= intensity;
        yaw_cam -= xoffset;
        check_pitch();
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_cam)) * cos(glm::radians(pitch_cam));
        front.y = sin(glm::radians(pitch_cam));
        front.z = sin(glm::radians(yaw_cam)) * cos(glm::radians(pitch_cam));
        cameraFront = glm::normalize(front);
    }
    if (keys[GLFW_KEY_D] || (timeDiff < pressedTime && lastkey=='D')){
        lastkey='D';
        //cameraPos -= glm::normalize(glm::cross(cameraFront , cameraUp)) * cameraSpeed
        GLfloat xoffset = cameraSpeed* 30;    // Change this value to your liking
        if (timeDiff>0 && intensity>0)
            xoffset *= intensity;
        yaw_cam += xoffset;
        check_pitch();
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_cam)) * cos(glm::radians(pitch_cam));
        front.y = sin(glm::radians(pitch_cam));
        front.z = sin(glm::radians(yaw_cam)) * cos(glm::radians(pitch_cam));
        cameraFront = glm::normalize(front);
    }

    if (keys[GLFW_KEY_Q] || (timeDiff < pressedTime && lastkey=='Q')){
        lastkey='Q';
        //cameraPos += glm::normalize(glm::cross(cameraFront , cameraUp)) * cameraSpeed;
        GLfloat yoffset = cameraSpeed * 30;
        if (timeDiff>0 && intensity>0)
            yoffset *= intensity;
        pitch_cam += yoffset;
        check_pitch();
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_cam)) * cos(glm::radians(pitch_cam));
        front.y = sin(glm::radians(pitch_cam));
        front.z = sin(glm::radians(yaw_cam)) * cos(glm::radians(pitch_cam));
        cameraFront = glm::normalize(front);
    }

    if (keys[GLFW_KEY_E] || (timeDiff < pressedTime && lastkey=='E')){
        lastkey='E';
        //cameraPos += glm::normalize(glm::cross(cameraFront , cameraUp)) * cameraSpeed;
        GLfloat yoffset = cameraSpeed * 30;
        if (timeDiff>0 && intensity>0)
            yoffset *= intensity;
        pitch_cam -= yoffset;
        check_pitch();
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_cam)) * cos(glm::radians(pitch_cam));
        front.y = sin(glm::radians(pitch_cam));
        front.z = sin(glm::radians(yaw_cam)) * cos(glm::radians(pitch_cam));
        cameraFront = glm::normalize(front);
    }
}

bool firstMouse = true;

void mouse_callback(GLFWwindow *window , double xpos , double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
    lastX = xpos;
    lastY = ypos;

    GLfloat sensitivity = 0.05;    // Change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw_cam += xoffset;
    pitch_cam += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch_cam > 89.0f)
        pitch_cam = 89.0f;
    if (pitch_cam < -89.0f)
        pitch_cam = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw_cam)) * cos(glm::radians(pitch_cam));
    front.y = sin(glm::radians(pitch_cam));
    front.z = sin(glm::radians(yaw_cam)) * cos(glm::radians(pitch_cam));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow *window , double xoffset , double yoffset) {
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}


int main(int argc , char *argv[]) {
    // GLFW Initialization
    if (!glfwInit()) {
        fprintf(stderr , "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR , 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR , 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT , GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE , GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow *window = glfwCreateWindow(window_width , window_height ,
                                          "ICG project 1.0" , NULL , NULL);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window , key_callback);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window , SetupProjection);

    // set the mouse press and position callback
    //glfwSetCursorPosCallback(window, MousePos);
    glfwSetScrollCallback(window , scroll_callback);

    // GLFW Options
    glfwSetInputMode(window , GLFW_CURSOR , GLFW_CURSOR_DISABLED);

    // Set the required callback functions
    glfwSetCursorPosCallback(window , mouse_callback);

    // GLFW Options

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if (glewInit() != GLEW_NO_ERROR) {
        fprintf(stderr , "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init(window);

    // update the window size with the framebuffer size (on hidpi screens the
    // framebuffer is bigger)
    glfwGetFramebufferSize(window , &window_width , &window_height);
    SetupProjection(window , window_width , window_height);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        move_terrain();
        // update mirror camera postion
        cam_pos_mirr = vec3(cameraPos.x, -cameraPos.y, cameraPos.z);

        // Camera/View transformation
        glm::mat4 view;
        view_matrix = glm::lookAt(cameraPos , cameraPos + cameraFront , cameraUp);

        // Projection
        // glm::mat4 projection = glm::r(fov, (GLfloat)window_width/(GLfloat)window_height, 0.1f, 100.0f);
        // Get the uniform locations
        //GLint projLoc = glGetUniformLocation(grid., "projection");
        //glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        if(cameraMode==CAM_FPS){
            if(tex[0]<.2)
                tex[0]=.2;
            cameraPos.y=tex[0]+.5;
        }

        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();


    }
    quad.Cleanup();
    grid.Cleanup();
    skybox.Cleanup();
    water.Cleanup();
    tree.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
