// Naimur Rahman
// Roll: 1907031
// 3D Classroom Assignment

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "basic_camera.h"
#include "camera.h"

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawTableChair(unsigned int VAO, Shader ourShader, glm::mat4 matrix);
void drawFan(unsigned int VAO, Shader ourShader, glm::mat4 matrix);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 2.0f, eyeY = 0.0f, eyeZ = 13.5f;
float lookAtX = 2.0f, lookAtY = 1.5f, lookAtZ = 4.75f;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 eye = glm::vec3(eyeX, eyeY, eyeZ);
glm::vec3 lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);
Camera camera(glm::vec3(eyeX, eyeY, eyeZ));

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

float r = 0.0f;
bool fanOn = false;
float aspectRatio = 4.0f / 3.0f;

bool birdEyeView = false;
glm::vec3 birdEyePosition(2.0f, 3.5f, 13.5f); // Initial position (10 units above)
glm::vec3 birdEyeTarget(2.0f, 0.0f, 7.5f);   // Focus point
float birdEyeSpeed = 1.0f;                   // Speed of movement

float theta = 0.0f; // Angle around the Y-axis
float radius = 2.0f; // Distance from lookAt point

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "1907031: Assignment 2", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    Shader constantShader("vertexShader.vs", "fragmentShaderV2.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cube_vertices[] = {
        0.0f, 0.0f, 0.0f, 0.647f, 0.408f, 0.294f,
        0.5f, 0.0f, 0.0f, 0.647f, 0.408f, 0.294f,
        0.5f, 0.5f, 0.0f, 0.647f, 0.408f, 0.294f,
        0.0f, 0.5f, 0.0f, 0.647f, 0.408f, 0.294f,

        0.0f, 0.0f, 0.5f, 0.647f, 0.408f, 0.294f,
        0.5f, 0.0f, 0.5f, 0.647f, 0.408f, 0.294f,
        0.5f, 0.5f, 0.5f, 0.647f, 0.408f, 0.294f,
        0.0f, 0.5f, 0.5f, 0.647f, 0.408f, 0.294f,
    
    };
    
    unsigned int cube_indices[] = {
        1, 2, 3,
        3, 0, 1,

        5, 6, 7,
        7, 4, 5,

        4, 7, 3,
        3, 0, 4,

        5, 6, 2,
        2, 1, 5,

        5, 1, 0,
        0, 4, 5,

        6, 2, 3,
        3, 7, 6,
    };
    
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    ourShader.use();
    //constantShader.use();

    r = 0.0f;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        //glm::mat4 projection = glm::perspective(glm::radians(basic_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);
        //constantShader.setMat4("projection", projection);

        // camera/view transformation
        //glm::mat4 view = basic_camera.createViewMatrix();
        //ourShader.setMat4("view", view);
        //constantShader.setMat4("view", view);

        glm::mat4 view;

        if (birdEyeView) {
            // Set camera position directly above the scene
            glm::vec3 up(0.0f, 1.0f, 0.0f); // Ensure the up vector points backward
            view = glm::lookAt(birdEyePosition, birdEyeTarget, up);
        }
        else {
            view = camera.GetViewMatrix();
        }

        ourShader.setMat4("view", view);
        // Modelling Transformation
        //glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        //drawCube(ourShader, VAO, identityMatrix, translate_X, translate_Y, translate_Z, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, scale_X, scale_Y, scale_Z);
        /*
        glm::mat4 identityMatrix = glm::mat4(1.0f);
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = identityMatrix;
        */
        //model = glm::scale(rotateZMatrix, glm::vec3(scX, scY, scZ));
        //modelCentered = glm::translate(model, glm::vec3(-0.25, -0.25, -0.25));

        //shaderProgram.setMat4("model", modelCentered);

        glm::mat4 identityMatrix = glm::mat4(1.0f);
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, RotateTranslateMatrix, InvRotateTranslateMatrix;
        
        //draw tables and chairs
        float z = 0.0f;
        for (int j = 0; j < 5; j++) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, z));
            drawTableChair(VAO, ourShader, translateMatrix);

            for (int i = 0; i < 2; i++) {
                translateMatrix = translateMatrix * glm::translate(identityMatrix, glm::vec3(-1.25f, 0.0f, 0.0f));
                drawTableChair(VAO, ourShader, translateMatrix);
            }

            translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 0.0f, z));
            drawTableChair(VAO, ourShader, translateMatrix);

            for (int i = 0; i < 2; i++) {
                translateMatrix = translateMatrix * glm::translate(identityMatrix, glm::vec3(1.25f, 0.0f, 0.0f));
                drawTableChair(VAO, ourShader, translateMatrix);
            }

            z += 2.25;
        }

        //faculty table
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.5f, 0.0f, -0.75f));
        drawTableChair(VAO, ourShader, translateMatrix * rotateYMatrix);

        //standing structure top
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.0f, -0.5f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(12.0f, 0.2f, 6.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.624f, 0.416f, 0.310f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        //standing structure back left leg
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.0f, -0.5f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.455f, 0.235f, 0.102f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        //standing structure front left leg
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.0f, -0.5f, -1.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        //standing structure back right leg
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.9f, -0.5f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        //standing structure front right leg
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.9f, -0.5f, -1.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //floor
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.2f, -1.0f, -4.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(20.8f, -0.2f, 35.8f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.494f, 0.514f, 0.541f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //front wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.2f, -1.0f, -4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(20.8f, 9.0f, -0.2f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.659f, 0.820f, 0.843f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //left wall section 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, -1.0f, -4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 9.0f, 9.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //left wall window 1 down section
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, -1.0f, 0.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 2.0f, 5.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //left wall window 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, 0.0f, 0.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 5.0f, 5.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.404f, 0.353f, 0.325f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //left wall window 1 up section
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, 2.5f, 0.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 2.0f, 5.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.659f, 0.820f, 0.843f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //left wall section 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, -1.0f, 2.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 9.0f, 9.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //left wall window 2 down section
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, -1.0f, 7.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 2.0f, 5.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //left wall window 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, 0.0f, 7.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 5.0f, 5.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.404f, 0.353f, 0.325f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //left wall window 2 up section
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, 2.5f, 7.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 2.0f, 5.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.659f, 0.820f, 0.843f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //left wall section 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, -1.0f, 9.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 9.0f, 7.4f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //right wall section 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, -1.0f, -4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 9.0f, 12.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //right wall window down section
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, -1.0f, 1.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 2.0f, 5.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //right wall window
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, 0.0f, 1.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 5.0f, 5.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.404f, 0.353f, 0.325f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //right wall window up section
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, 2.5f, 1.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 2.0f, 5.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.659f, 0.820f, 0.843f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //right wall section 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, -1.0f, 4.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 9.0f, 13.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //right wall door up section
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, 2.0f, 10.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 3.0f, 4.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //door
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, -1.0f, 10.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 6.0f, 4.0f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.404f, 0.353f, 0.325f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //right wall section 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, -1.0f, 12.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 9.0f, 1.4f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.659f, 0.820f, 0.843f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);       

        //back wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.2f, -1.0f, 13.7f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(20.8f, 9.0f, -0.2f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //whiteboard
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.5f, 0.5f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.0f, 4.0f, -0.2f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        //roof
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.2f, 3.5f, -4.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(20.8f, 0.2f, 35.8f));
        model = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.494f, 0.514f, 0.541f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //draw fans with rotations
        z = 0.0f;
        for (int i = 0; i < 3; i++) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, z));
            drawFan(VAO, ourShader, translateMatrix);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(3.33f, 0.0f, z));
            drawFan(VAO, ourShader, translateMatrix);

            z += 3.5;
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void drawFan(unsigned int VAO, Shader ourShader, glm::mat4 matrix) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, RotateTranslateMatrix, InvRotateTranslateMatrix;

    if (fanOn) {
        //fan rod
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.28f, 3.5f, 1.61f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.5f, 0.2f));
        model = matrix * translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan middle
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.13f, 2.75f, 1.46f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2f, 0.0f, -0.2f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.2f, 0.0f, 0.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, -0.2f, 0.8f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars left
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.13f, 2.75f, 1.56f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2f, 0.0f, -0.1f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.2f, 0.0f, 0.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.5f, -0.2f, 0.4f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars right
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.53f, 2.75f, 1.56f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.2f, 0.0f, -0.1f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2f, 0.0f, 0.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, -0.2f, 0.4f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars up
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.23f, 2.75f, 1.46f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.1f, 0.0f, -0.2f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, 0.0f, 0.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, -0.2f, -1.5f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars down
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.23f, 2.75f, 1.86f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.1f, 0.0f, 0.2f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, 0.0f, -0.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, -0.2f, 1.5f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        r += 0.5f;
    }

    else {
        //fan rod
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.28f, 3.5f, 1.61f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.5f, 0.2f));
        model = matrix * translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan middle
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.13f, 2.75f, 1.46f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, -0.2f, 0.8f));
        model = matrix * translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars left
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.13f, 2.75f, 1.56f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.5f, -0.2f, 0.4f));
        model = matrix * translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);
        ourShader.setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars right
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.53f, 2.75f, 1.56f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, -0.2f, 0.4f));
        model = matrix * translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars up
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.23f, 2.75f, 1.46f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, -0.2f, -1.5f));
        model = matrix * translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //fan propelars down
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.23f, 2.75f, 1.86f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, -0.2f, 1.5f));
        model = matrix * translateMatrix * scaleMatrix;
        ourShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

void drawTableChair(unsigned int VAO, Shader ourShader, glm::mat4 matrix) {
    //table top
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 0.2f, 2.0f));
    model = matrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.882f, 0.710f, 0.604f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //table back
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, -2.0f, 0.2f));
    model = matrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.647f, 0.408f, 0.294f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //table leg left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -2.0f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //table leg right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, 0.0f, 0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -2.0f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //table mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -0.75f, 0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 0.2f, 1.6f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.882f, 0.710f, 0.604f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //PC
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, -0.65f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 1.0f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //monitor base
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.4f, 0.1f, 0.2f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 0.1f, 0.4f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //monitor pipe
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.45f, 0.15f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //monitor screen
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.6f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //keyboard
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.15f, 0.1f, 0.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 0.5f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //mouse
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8f, 0.1f, 0.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 1.0f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.455f, 0.235f, 0.102f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair leg back left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);
    ourShader.setVec4("color", glm::vec4(0.329f, 0.173f, 0.110f, 1.0f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair leg front left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.5f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair leg front right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.65f, -0.5f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair leg back right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.65f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -1.0f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair upper piller left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.4f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.3f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair upper piller right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.65f, -0.4f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.3f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair upper line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, 0.15f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //chair upper mid line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.20f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        if (fanOn) {
            fanOn = false;
        }
        else {
            fanOn = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        birdEyeView = !birdEyeView;
    }

    if (birdEyeView) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            birdEyePosition.z -= birdEyeSpeed * deltaTime; // Move forward along Z
            birdEyeTarget.z -= birdEyeSpeed * deltaTime;
            if (birdEyePosition.z <= 2.0) {
                birdEyePosition.z = 2.0;
            }
            if (birdEyeTarget.z <= -4.0) {
                birdEyeTarget.z = -4.0;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            birdEyePosition.z += birdEyeSpeed * deltaTime; // Move backward along Z
            birdEyeTarget.z += birdEyeSpeed * deltaTime;
            if (birdEyePosition.z >= 13.5) {
                birdEyePosition.z = 13.5;
            }
            if (birdEyeTarget.z >= 7.5) {
                birdEyeTarget.z = 7.5;
            }
        }
    }
    
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {   
        if (!birdEyeView) {
            theta += 0.01f; // Increment angle to rotate
            // Calculate new eye position using spherical coordinates
            camera.Position.x = lookAtX + radius * sin(theta);
            camera.Position.y = lookAtY;
            camera.Position.z = lookAtZ + radius * cos(theta);
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    float aspectRatio = 4.0f / 3.0f;
    int viewWidth, viewHeight;

    if (width / (float)height > aspectRatio) {
        // Window is too wide, fit height and adjust width
        viewHeight = height;
        viewWidth = (int)(height * aspectRatio);
    }
    else {
        // Window is too tall, fit width and adjust height
        viewWidth = width;
        viewHeight = (int)(width / aspectRatio);
    }

    // Center the viewport
    int xOffset = (width - viewWidth) / 2;
    int yOffset = (height - viewHeight) / 2;

    glViewport(xOffset, yOffset, viewWidth, viewHeight);
    //glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}