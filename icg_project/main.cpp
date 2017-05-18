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
#include "imgui-master/imgui.h"
#include "imgui-master/imgui_impl_glfw_gl3.h"

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
float cameraSpeed_F=.25f;

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

//skybox rotation scale
float sky_rspeed = 0.02;

GLfloat tex[TEX_BITS]; // window height * window width * floats per pixel

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
    float cameraSpeed = cameraSpeed_F * deltaTime;
    if(cameraMode==CAM_FPS)
        cameraSpeed = cameraSpeed_F*.1*deltaTime;

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
        GLfloat xoffset = cameraSpeed* 300;    // Change this value to your liking
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
        GLfloat xoffset = cameraSpeed* 300;    // Change this value to your liking
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
        GLfloat yoffset = cameraSpeed * 300;
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
        GLfloat yoffset = cameraSpeed * 300;
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
    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);

    bool show_camera_window = true;
    bool default_window = true;
    ImVec4 clear_color = ImColor(114, 144, 154);

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

        ImGui_ImplGlfwGL3_NewFrame();

        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        if(default_window)
        {
            ImGui::Begin("GUI", &default_window);
            ImGui::SetNextWindowSize(ImVec2(550,680), ImGuiSetCond_FirstUseEver);
            if (ImGui::CollapsingHeader("Terrain Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderInt("Octaves", &octaves, 0, 20);
                ImGui::SliderFloat("Frequency", &frequency, 0.0f, 5.0f);
                ImGui::SliderFloat("Amplitude", &amplitude, 0.0f, 5.0f);
                ImGui::SliderFloat("Lacunarity", &lacunarity, 0.0f, 5.0f);
                ImGui::SliderFloat("Persistance", &persistance, 0.0f, 5.0f);
                ImGui::SliderFloat("H", &H, 0.0f, 5.0f);
            }
            if (ImGui::CollapsingHeader("Camera Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderFloat("Camera Speed", &cameraSpeed_F, 0.0f, 5.0f);
            }
            if (ImGui::CollapsingHeader("Help"))
            {
                ImGui::TextWrapped("Navigation:\n");
                ImGui::BulletText("Use the keys W and A to move back and forth");
                ImGui::BulletText("Use the keys Q and E to look up and down");
                ImGui::BulletText("Use the keys A and D to look right and left");
                ImGui::TextWrapped("Camera:\n");
                ImGui::BulletText("Use the key F to toggle FPS (first-person shooter) navigation mode");
                ImGui::TextWrapped("GUI:\n");
                ImGui::ShowUserGuide();
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_camera_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_camera_window);
        }
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        Display();
        ImGui::Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    quad.Cleanup();
    grid.Cleanup();
    skybox.Cleanup();
    water.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
