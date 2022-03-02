#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;
glm::mat4 lightRotation;


// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;
glm::vec3 lightBuild;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLint lightBuildLoc;

// camera
gps::Camera myCamera(
    glm::vec3(55.0f, 70.0f, 480.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 4.0f;

GLboolean pressedKeys[1024];

// models
gps::Model3D runway;
gps::Model3D plane;
gps::Model3D planeCopy1;
gps::Model3D planeCopy2;
gps::Model3D tower;
gps::Model3D pavement;
gps::Model3D build;
gps::Model3D elice;
gps::Model3D helicopter;
gps::Model3D bus;
gps::Model3D terminal;
gps::Model3D bird;
gps::Model3D man;
gps::Model3D reflector;

GLfloat angle;

//gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D sun;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

// shaders
gps::Shader myBasicShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

std::vector<const GLchar*> faces;


gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

float angleLight = 0.0f;
float angleMan = 0.0f;
float delta = 0.0f;
float rotatePlane = 0.0f;
//float deltaH = 0.0f;
float deltaR = 0.0f;
float rotateElice = true;
double rotateCamera = -0.2f;
double moveBus = 0.0f;

// camera animation (preview)
bool startPreview = false;
// fog
GLint foginitLoc;
GLfloat fogDensity = 0.0f;

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    //TODO

}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_I && action == GLFW_PRESS)
        showDepthMap = !showDepthMap;

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

double lastX, lastY;
double yaw, pitch;
bool mouseMoved = true;
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    //TODO

    GLfloat sensitivity = 0.1f;

    GLfloat xoffset = lastX - xpos;
    GLfloat yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    xoffset *= rotateCamera;
    yoffset *= rotateCamera;

    yaw += xoffset;
    pitch += yoffset;

    mouseMoved = true;

    myCamera.rotate(pitch, yaw);
}
void functieVizualizareScena() {
    if (startPreview) {  
        myCamera.vizualizareScena();
    }
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  
}


void processMovement() {
    if (pressedKeys[GLFW_KEY_W]) { //INAINTE
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_S]) { //INAPOI
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A]) { //STANGA
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_D]) { //DREAPTA
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_Z]) { //IN SUS
        myCamera.move(gps::MOVE_UP, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        //// compute normal matrix for teapot
        //normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_X]) {//IN JOS
        myCamera.move(gps::MOVE_DOWN, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        //// compute normal matrix for teapot
        //normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_B]) //Line view
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (pressedKeys[GLFW_KEY_N]) //Normal view
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (pressedKeys[GLFW_KEY_M]) //Point view
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    if (pressedKeys[GLFW_KEY_K]) { // move ligth left
        angleLight -= 1.0f;
        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleLight), glm::vec3(0.0f, 0.5f, 1.0f));

        myBasicShader.useShaderProgram();
        lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
    }

    if (pressedKeys[GLFW_KEY_L]) { // move light right
        angleLight += 1.0f;
        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleLight), glm::vec3(0.0f, 0.5f, 1.0f));
        myBasicShader.useShaderProgram();
        lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

        lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
    }

    if (pressedKeys[GLFW_KEY_C]) { //Miscare avion in spate
        delta += 0.5;
    }

    if (pressedKeys[GLFW_KEY_V]) { //Miscare avion in fata
        delta -= 1, 0;
    }

    if (pressedKeys[GLFW_KEY_F]) { //Miscare avion in sus
        rotatePlane += 0.25;
    }

    if (pressedKeys[GLFW_KEY_G]) { //Miscare avion in jos
        rotatePlane -= 0.25;
    }

    // start preview
    if (pressedKeys[GLFW_KEY_T]) {
        startPreview = true;
    }

    // stop preview
    if (pressedKeys[GLFW_KEY_Y]) {
       startPreview = false;
    }

    //Cresterea densitatii cetei
    if (pressedKeys[GLFW_KEY_H]) {
        fogDensity += 0.0001f;
        if (fogDensity >= 0.1f) {
            fogDensity = 0.1f;
        }
        myBasicShader.useShaderProgram();
        glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity"), fogDensity);
    }
    //Micsorarea densitatii
    if (pressedKeys[GLFW_KEY_J]) {
        fogDensity -= 0.0001f;
        if (fogDensity <= 0.0f) {
            fogDensity = 0.0f;
        }
        myBasicShader.useShaderProgram();
        glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity"), fogDensity);
    }

    if (pressedKeys[GLFW_KEY_O]) { //Miscare autobuz in fata
        moveBus += 0.9;
    }

    if (pressedKeys[GLFW_KEY_P]) { //Miscare autobuz in spate
        moveBus -= 0.9;
    }
    if (pressedKeys[GLFW_KEY_Q]) { //Rotire barbat
        angleMan += 0.2f;
    }

    if (pressedKeys[GLFW_KEY_E]) { //Rotire barbat
        angleMan -= 0.2f;
    }

    if (pressedKeys[GLFW_KEY_1]) {
        glDisable(GL_MULTISAMPLE);
        //Turn on smoothing of vertices
        glEnable(GL_POINT_SMOOTH);
    }
    if (pressedKeys[GLFW_KEY_2]) {
        glDisable(GL_POINT_SMOOTH);
        //Turn on multisampling
        glEnable(GL_MULTISAMPLE);
    }
}

void initOpenGLWindow() {
    myWindow.Create(2048, 1024, "OpenGL Project Core");
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetScrollCallback(myWindow.getWindow(), scroll_callback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);         


}

void initModels() {

    plane.LoadModel("models/plane/plane2.obj");
    planeCopy1.LoadModel("models/plane/planeSimple1.obj");
    planeCopy2.LoadModel("models/plane/planeSimple2.obj");
    runway.LoadModel("models/road/runway.obj");
    tower.LoadModel("models/tower/tower.obj");
    bus.LoadModel("models/car/bus.obj");
    pavement.LoadModel("models/grass/pavement.obj");
    build.LoadModel("models/building/building.obj");
    terminal.LoadModel("models/building/terminal.obj");
    bird.LoadModel("models/hawk/hawk.obj");
    sun.LoadModel("models/sun/sun.obj");
    screenQuad.LoadModel("models/quad/quad.obj");
    man.LoadModel("models/man/man.obj");
    reflector.LoadModel("models/light/reflector.obj");
}

void initShaders() {
    myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    screenQuadShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
    depthMapShader.useShaderProgram();
}

void initUniforms() {

    faces.push_back("textures/skybox/cloudtop_rt.tga");
    faces.push_back("textures/skybox/cloudtop_lf.tga");
    faces.push_back("textures/skybox/cloudtop_up.tga");
    faces.push_back("textures/skybox/cloudtop_dn.tga");
    faces.push_back("textures/skybox/cloudtop_bk.tga");
    faces.push_back("textures/skybox/cloudtop_ft.tga");
    myBasicShader.useShaderProgram();

    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity"), fogDensity);

    mySkyBox.Load(faces);
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();


    //create matrix for skybox
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
        glm::value_ptr(view));

    projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 20.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
        glm::value_ptr(projection));
    myBasicShader.useShaderProgram();
    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 2000.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection)); ;


    //set the light direction (direction towards the light)
    lightDir = glm::vec3(-50.0f, 200.0f, 60.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleLight), glm::vec3(0.0f, 0.0f, 1.0f));
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");

    // pointlight
    lightBuild = glm::vec3(20.0f, 0.0f, 0.0f); // la bus
    lightBuildLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPosEye");
    glUniform3fv(lightBuildLoc, 1, glm::value_ptr(lightBuild));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}
void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
    glGenFramebuffers(1, &shadowMapFBO);
    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix

    glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 20.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;
}


bool movePlane = false;
float deltaH = 2000;
float birdMove = 200.0f;
double lastTime = glfwGetTime();

void moveBird(double varSecond) {
    deltaH = deltaH - birdMove * varSecond;
    if (deltaH < -3000)
    {
        deltaH = 2000;
    }
}
void renderObjects(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //Plane objects
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 4.5 * rotatePlane, delta));
    model = glm::rotate(model, glm::radians(rotatePlane), glm::vec3(1.0f, 0.0f, 0.0f));
    //send matrix data to vertex shader 
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    plane.Draw(shader);

    //Plane objects
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    planeCopy1.Draw(shader);
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    planeCopy2.Draw(shader);

    //Runway object
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    runway.Draw(shader);


    //Pavement object
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(17.0f, -5.0f, 150.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, glm::radians(-1.2f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    pavement.Draw(shader);

    //Tower object
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    tower.Draw(shader);


    //Build object
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    build.Draw(shader);

    //Terminal object
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    terminal.Draw(shader);

    //Bus object
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, moveBus));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    bus.Draw(shader);

    //Bird object
    double currentTime = glfwGetTime();
    moveBird(currentTime - lastTime);
    lastTime = currentTime;
    model = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f + deltaH, 200.3f, -800.0f));

    model = glm::scale(model, glm::vec3(2.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    bird.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angleMan), glm::vec3(0.0f, 1.0f, 0.0f));
    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    man.Draw(shader);

    model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    reflector.Draw(shader);

}

void renderScene() {

    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderObjects(depthMapShader, false);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity"), fogDensity);
    functieVizualizareScena();

    if (showDepthMap) {
        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    }
    else {

        // final scene rendering pass (with shadows)

        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myBasicShader.useShaderProgram();

        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleLight), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

        //bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(computeLightSpaceTrMatrix()));
        /*glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity"), fogDensity)*/;
        renderObjects(myBasicShader, false);


        //draw a white cube around the light

        lightShader.useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));


        model = lightRotation;
        model = glm::translate(model, 1.0f * lightDir);
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        sun.Draw(lightShader);
    }
    mySkyBox.Draw(skyboxShader, view, projection);

}

void cleanup() {
    glDeleteTextures(1, &depthMapTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &shadowMapFBO);
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    initFBO();
    setWindowCallbacks();

    glCheckError();
    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        renderScene();
        glCheckError();
        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());


    }

    cleanup();

    return EXIT_SUCCESS;
}
