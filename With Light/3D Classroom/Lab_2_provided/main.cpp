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
#include "pointLight.h"

#include <iostream>

using namespace std;

//function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawTableChair(unsigned int VAO, Shader lightingShader, glm::mat4 matrix);
void drawFan(unsigned int VAO, Shader lightingShader, glm::mat4 matrix);
void drawCube(unsigned int& VAO, Shader& lightingShader, glm::mat4 model, glm::vec3 color);

//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//screen
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float aspectRatio = 4.0f / 3.0f;

//camera
float eyeX = 2.0f, eyeY = 1.0f, eyeZ = 13.5f;
float lookAtX = 2.0f, lookAtY = 1.5f, lookAtZ = 4.75f;
Camera camera(glm::vec3(eyeX, eyeY, eyeZ));

//timing
float deltaTime = 0.0f;     // time between current frame and last frame
float lastFrame = 0.0f;

//fan
float r = 0.0f;
bool fanOn = false;

//bird's eye view
bool birdEyeView = false;
glm::vec3 birdEyePosition(2.0f, 3.5f, 13.5f);
glm::vec3 birdEyeTarget(2.0f, 0.0f, 7.5f);
float birdEyeSpeed = 1.0f;

//rotation around a point
float theta = 0.0f; // Angle around the Y-axis
float radius = 2.0f;

//directional light
bool directionLightOn = true;
bool directionalAmbient = true;
bool directionalDiffuse = true;
bool directionalSpecular = true;

//spot light
bool spotLightOn = true;

//point light
bool point1 = true;
bool point2 = true;

//custom projection matrix
float fov = glm::radians(camera.Zoom);
float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
float near = 0.1f;
float far = 100.0f;
float tanHalfFOV = tan(fov / 2.0f);

//positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(2.0f,  2.0f,  2.0f),
    glm::vec3(2.0f,  2.0f,  8.0f),
};

PointLight pointlight1(
    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,       // position
    0.2f, 0.2f, 0.2f,       //ambient
    0.8f, 0.8f, 0.8f,       //diffuse
    1.0f, 1.0f, 1.0f,       //specular
    1.0f,       //k_c
    0.09f,      //k_l
    0.032f,     //k_q
    1       //light number
);

PointLight pointlight2(
    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,
    0.2f, 0.2f, 0.2f,
    0.8f, 0.8f, 0.8f,
    1.0f, 1.0f, 1.0f,
    1.0f,
    0.09f,
    0.032f,
    2
);

int main()
{
    //glfw initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "1907031: Assignment 3", NULL, NULL);
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

    //tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //glad load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //configure global opengl state
    glEnable(GL_DEPTH_TEST);

    //build and compile our shader program
    Shader lightingShader("vertexShaderForGouraudShading.vs", "fragmentShaderForGouraudShading.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");
    Shader constantShader("vertexShader.vs", "fragmentShaderV2.fs");

    //set up vertex data (and buffer(s)) and configure vertex attributes
    float cube_vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };
    
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
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

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    //second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    //note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //ourShader.use();
    //constantShader.use();

    r = 0.0f;

    //render loop
    while (!glfwWindowShouldClose(window))
    {
        //per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //input
        processInput(window);

        //render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //ourShader.use();
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        //point lights set up
        pointlight1.setUpPointLight(lightingShader);
        pointlight2.setUpPointLight(lightingShader);

        //directional light set up
        lightingShader.setVec3("directionalLight.direction", 0.0f, -1.0f, 0.0f);
        lightingShader.setVec3("directionalLight.ambient", 0.1f, 0.1f, 0.1f);
        lightingShader.setVec3("directionalLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("directionalLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setBool("directionLightOn", directionLightOn);

        //spot light set up
        lightingShader.setVec3("spotLight.position", 2.0f, 2.0f, -3.0f);
        lightingShader.setVec3("spotLight.direction", 0.0f, -1.0f, 0.0f);
        lightingShader.setVec3("spotLight.ambient", 0.5f, 0.5f, 0.5f);
        lightingShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.k_c", 1.0f);
        lightingShader.setFloat("spotLight.k_l", 0.09);
        lightingShader.setFloat("spotLight.k_q", 0.032);
        lightingShader.setFloat("spotLight.cos_theta", glm::cos(glm::radians(60.0f)));
        lightingShader.setBool("spotLightOn", spotLightOn);

        //handle for changes in directional light directly from shedder
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            if (directionLightOn) {
                lightingShader.setBool("ambientLight", !directionalAmbient);
                directionalAmbient = !directionalAmbient;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
            if (directionLightOn) {
                lightingShader.setBool("diffuseLight", !directionalDiffuse);
                directionalDiffuse = !directionalDiffuse;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
            if (directionLightOn) {
                lightingShader.setBool("specularLight", !directionalSpecular);
                directionalSpecular = !directionalSpecular;
            }
        }

        glm::mat4 projection(0.0f);
        projection[0][0] = 1.0f / (aspect * tanHalfFOV);
        projection[1][1] = 1.0f / tanHalfFOV;
        projection[2][2] = -(far + near) / (far - near);
        projection[2][3] = -1.0f;
        projection[3][2] = -(2.0f * far * near) / (far - near);
        //pass projection matrix to shader (note that in this case it could change every frame)
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        //ourShader.setMat4("projection", projection);
        //constantShader.setMat4("projection", projection);
        lightingShader.setMat4("projection", projection);

        //camera view transformation
        //constantShader.setMat4("view", view);
        //ourShader.setMat4("view", view);

        glm::mat4 view;

        //check for bird's eye view and normal view
        if (birdEyeView) {
            glm::vec3 up(0.0f, 1.0f, 0.0f);
            view = glm::lookAt(birdEyePosition, birdEyeTarget, up);
        }
        else {
            view = camera.GetViewMatrix();
        }
        
        lightingShader.setMat4("view", view);

        //define matrices and vectors needed
        glm::mat4 identityMatrix = glm::mat4(1.0f);
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, RotateTranslateMatrix, InvRotateTranslateMatrix;
        glm::vec3 color;
        
        //initialize all elements as non-emissive
        lightingShader.setVec3("material.emissive", glm::vec3(0.0f, 0.0f, 0.0f));

        //draw tables and chairs
        float z = 0.0f;
        for (int j = 0; j < 5; j++) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, z));
            drawTableChair(VAO, lightingShader, translateMatrix);

            for (int i = 0; i < 2; i++) {
                translateMatrix = translateMatrix * glm::translate(identityMatrix, glm::vec3(-1.25f, 0.0f, 0.0f));
                drawTableChair(VAO, lightingShader, translateMatrix);
            }

            translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 0.0f, z));
            drawTableChair(VAO, lightingShader, translateMatrix);

            for (int i = 0; i < 2; i++) {
                translateMatrix = translateMatrix * glm::translate(identityMatrix, glm::vec3(1.25f, 0.0f, 0.0f));
                drawTableChair(VAO, lightingShader, translateMatrix);
            }

            z += 2.25;
        }

        //faculty table
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.5f, 0.0f, -0.75f));
        drawTableChair(VAO, lightingShader, translateMatrix * rotateYMatrix);

        //standing structure top
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.0f, -0.5f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(6.0f, 0.1f, 3.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.624f, 0.416f, 0.310f);
        drawCube(VAO, lightingShader, model, color);
        
        //standing structure back left leg
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.0f, -0.5f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.5f, 0.1f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.455f, 0.235f, 0.102f);
        drawCube(VAO, lightingShader, model, color);
        
        //standing structure front left leg
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.0f, -0.5f, -1.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.5f, 0.1f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.455f, 0.235f, 0.102f);
        drawCube(VAO, lightingShader, model, color);
        
        //standing structure back right leg
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.9f, -0.5f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.5f, 0.1f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.455f, 0.235f, 0.102f);
        drawCube(VAO, lightingShader, model, color);
        
        //standing structure front right leg
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.9f, -0.5f, -1.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.5f, 0.1f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.455f, 0.235f, 0.102f);
        drawCube(VAO, lightingShader, model, color);

        //floor
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.2f, -1.0f, -4.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.4f, -0.1f, 17.9f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        drawCube(VAO, lightingShader, model, color);

        //front wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.2f, -1.0f, -4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.4f, 4.5f, -0.1f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //left wall section 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, -1.0f, -4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 4.5f, 4.5f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //left wall window 1 down section
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, -1.0f, 0.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 1.0f, 2.5f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //left wall window 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.15f, 0.0f, 0.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.05f, 2.5f, 2.5f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.404f, 0.353f, 0.325f);
        drawCube(VAO, lightingShader, model, color);

        //left wall window 1 up section
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, 2.5f, 0.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 1.0f, 2.5f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //left wall section 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, -1.0f, 2.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 4.5f, 4.5f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //left wall window 2 down section
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, -1.0f, 7.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 1.0f, 2.5f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //left wall window 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.15f, 0.0f, 7.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.05f, 2.5f, 2.5f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.404f, 0.353f, 0.325f);
        drawCube(VAO, lightingShader, model, color);

        //left wall window 2 up section
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, 2.5f, 7.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 1.0f, 2.5f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //left wall section 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.1f, -1.0f, 9.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 4.5f, 3.7f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //right wall section 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, -1.0f, -4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 4.5f, 6.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //right wall window down section
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, -1.0f, 1.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 1.0f, 2.5f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //right wall window
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, 0.0f, 1.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.05f, 2.5f, 2.5f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.404f, 0.353f, 0.325f);
        drawCube(VAO, lightingShader, model, color);

        //right wall window up section
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, 2.5f, 1.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 1.0f, 2.5f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //right wall section 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, -1.0f, 4.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 4.5f, 6.5f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //right wall door up section
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, 2.0f, 10.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 1.5f, 2.0f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //door
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, -1.0f, 10.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.05f, 3.0f, 2.0f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.404f, 0.353f, 0.325f);
        drawCube(VAO, lightingShader, model, color);

        //right wall section 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.2f, -1.0f, 12.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.1f, 4.5f, 0.7f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //back wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.2f, -1.0f, 13.7f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.4f, 4.5f, -0.1f));
        model = translateMatrix * scaleMatrix;
        //color = glm::vec3(0.659f, 0.820f, 0.843f);
        drawCube(VAO, lightingShader, model, color);

        //whiteboard
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.5f, 0.5f, -4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 2.0f, -0.1f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);
        
        //roof
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.2f, 3.5f, -4.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.4f, 0.1f, 17.9f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.494f, 0.514f, 0.541f);
        drawCube(VAO, lightingShader, model, color);

        //draw fans with rotations
        z = 0.0f;
        for (int i = 0; i < 3; i++) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, z));
            drawFan(VAO, lightingShader, translateMatrix);

            translateMatrix = glm::translate(identityMatrix, glm::vec3(3.33f, 0.0f, z));
            drawFan(VAO, lightingShader, translateMatrix);

            z += 3.5;
        }

        //light holder 1 with emissive material property
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.08f, 3.5f, 2.08f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.04f, -1.5f, 0.04f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.5f, 0.0f, 0.0f);

        lightingShader.setVec3("material.ambient", color);
        lightingShader.setVec3("material.diffuse", color);
        lightingShader.setVec3("material.specular", color);
        lightingShader.setVec3("material.emissive", color);
        lightingShader.setFloat("material.shininess", 32.0f);

        lightingShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //light holder 2 with emissive material property
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.08f, 3.5f, 8.08f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.04f, -1.5f, 0.04f));
        model = translateMatrix * scaleMatrix;
        color = glm::vec3(0.0f, 0.0f, 0.5f);

        lightingShader.setVec3("material.ambient", color);
        lightingShader.setVec3("material.diffuse", color);
        lightingShader.setVec3("material.specular", color);
        lightingShader.setVec3("material.emissive", color);
        lightingShader.setFloat("material.shininess", 32.0f);

        lightingShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //draw the lamp object(s)
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        //we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);

        for (unsigned int i = 0; i < 2; i++)
        {
            translateMatrix = glm::translate(identityMatrix, pointLightPositions[i]);
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -0.2f, 0.2f));
            model = translateMatrix * scaleMatrix;
            ourShader.setMat4("model", model);
            ourShader.setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        //glfw swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //optional: de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    //glfw terminate, clearing all previously allocated GLFW resources
    glfwTerminate();
    return 0;
}

void drawFan(unsigned int VAO, Shader lightingShader, glm::mat4 matrix) {
    //define matrices and vectors needed
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, RotateTranslateMatrix, InvRotateTranslateMatrix;
    glm::vec3 color;

    //when fan is on
    if (fanOn) {
        //fan rod
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.28f, 3.5f, 1.61f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.75f, 0.1f));
        model = matrix * translateMatrix * scaleMatrix;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan middle
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.13f, 2.75f, 1.46f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2f, 0.0f, -0.2f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.2f, 0.0f, 0.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, -0.1f, 0.4f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars left
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.13f, 2.75f, 1.56f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2f, 0.0f, -0.1f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.2f, 0.0f, 0.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.75f, -0.1f, 0.2f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars right
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.53f, 2.75f, 1.56f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.2f, 0.0f, -0.1f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.2f, 0.0f, 0.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.75f, -0.1f, 0.2f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars up
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.23f, 2.75f, 1.46f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.1f, 0.0f, -0.2f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, 0.0f, 0.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -0.1f, -0.75f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars down
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.23f, 2.75f, 1.86f));
        RotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(-0.1f, 0.0f, 0.2f));
        InvRotateTranslateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, 0.0f, -0.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -0.1f, 0.75f));
        model = matrix * translateMatrix * InvRotateTranslateMatrix * rotateYMatrix * RotateTranslateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        r += 1.0f;
    }

    //when fan is off
    else {
        //fan rod
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.28f, 3.5f, 1.61f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.75f, 0.1f));
        model = matrix * translateMatrix * scaleMatrix;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan middle
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.13f, 2.75f, 1.46f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, -0.1f, 0.4f));
        model = matrix * translateMatrix * scaleMatrix;
        color = glm::vec3(0.0f, 0.0f, 0.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars left
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.13f, 2.75f, 1.56f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.75f, -0.1f, 0.2f));
        model = matrix * translateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars right
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.53f, 2.75f, 1.56f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.75f, -0.1f, 0.2f));
        model = matrix * translateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars up
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.23f, 2.75f, 1.46f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -0.1f, -0.75f));
        model = matrix * translateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);

        //fan propelars down
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.23f, 2.75f, 1.86f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, -0.1f, 0.75f));
        model = matrix * translateMatrix * scaleMatrix;
        color = glm::vec3(1.0f, 1.0f, 1.0f);
        drawCube(VAO, lightingShader, model, color);
    }
}

void drawTableChair(unsigned int VAO, Shader lightingShader, glm::mat4 matrix) {
    //define matrices and vectors needed
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    glm::vec3 color;

    //table top
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 1.0f));
    model = matrix * scaleMatrix;
    color = glm::vec3(0.882f, 0.710f, 0.604f);
    drawCube(VAO, lightingShader, model, color);

    //table back
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, -1.0f, 0.1f));
    model = matrix * scaleMatrix;
    color = glm::vec3(0.647f, 0.408f, 0.294f);
    drawCube(VAO, lightingShader, model, color);

    //table leg left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.647f, 0.408f, 0.294f);
    drawCube(VAO, lightingShader, model, color);

    //table leg right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, 0.0f, 0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -1.0f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.647f, 0.408f, 0.294f);
    drawCube(VAO, lightingShader, model, color);

    //table mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -0.75f, 0.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 0.1f, 0.8f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.882f, 0.710f, 0.604f);
    drawCube(VAO, lightingShader, model, color);

    //PC
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, -0.65f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.25f, 0.5f, 0.5f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.0f, 0.0f, 0.0f);
    drawCube(VAO, lightingShader, model, color);

    //monitor base
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.4f, 0.1f, 0.2f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 0.05f, 0.2f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.0f, 0.0f, 0.0f);
    drawCube(VAO, lightingShader, model, color);

    //monitor pipe
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.45f, 0.15f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.0f, 0.0f, 0.0f);
    drawCube(VAO, lightingShader, model, color);

    //monitor screen
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.3f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.0f, 0.0f, 0.0f);
    drawCube(VAO, lightingShader, model, color);

    //keyboard
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.15f, 0.1f, 0.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.05f, 0.25f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.0f, 0.0f, 0.0f);
    drawCube(VAO, lightingShader, model, color);

    //mouse
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8f, 0.1f, 0.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.05f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.0f, 0.0f, 0.0f);
    drawCube(VAO, lightingShader, model, color);

    //chair mid section
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.1f, 0.5f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.455f, 0.235f, 0.102f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.5f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.5f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.5f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg front right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.65f, -0.5f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.5f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair leg back right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.65f, -0.5f, 1.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, -0.5f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair upper piller left
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.4f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.65f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair upper piller right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.65f, -0.4f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.65f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair upper line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, 0.15f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.1f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);

    //chair upper mid line
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.25f, -0.20f, 1.55f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.1f, 0.1f));
    model = matrix * translateMatrix * scaleMatrix;
    color = glm::vec3(0.329f, 0.173f, 0.110f);
    drawCube(VAO, lightingShader, model, color);
}

void drawCube(unsigned int& VAO, Shader& lightingShader, glm::mat4 model, glm::vec3 color)
{
    //use the shadder
    lightingShader.use();

    //define lighting properties
    lightingShader.setVec3("material.ambient", color);
    lightingShader.setVec3("material.diffuse", color);
    lightingShader.setVec3("material.specular", color);
    lightingShader.setFloat("material.shininess", 32.0f);

    lightingShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

//process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if(!birdEyeView)
            camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(P_UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(P_DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(Y_LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(Y_RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(R_LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        if (!birdEyeView)
            camera.ProcessKeyboard(R_RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        fanOn = !fanOn;
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
            theta += 0.01f;
            camera.Position.x = lookAtX + radius * sin(theta);
            camera.Position.y = lookAtY;
            camera.Position.z = lookAtZ + radius * cos(theta);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (pointlight1.ambientOn > 0 && pointlight1.diffuseOn > 0 && pointlight1.specularOn > 0) {
            pointlight1.turnOff();
            point1 = false;
        }    
        else {
            pointlight1.turnOn();
            point1 = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        if (pointlight2.ambientOn > 0 && pointlight2.diffuseOn > 0 && pointlight2.specularOn > 0) {
            pointlight2.turnOff();
            point2 = false;
        }            
        else {
            pointlight2.turnOn();
            point2 = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        directionLightOn = !directionLightOn;
    }

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        spotLightOn = !spotLightOn;
    }

    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        if (pointlight1.ambientOn > 0 || pointlight2.ambientOn > 0) {
            if(point1)
                pointlight1.turnAmbientOff();
            if(point2)
                pointlight2.turnAmbientOff();
        }
        else {
            if(point1)
                pointlight1.turnAmbientOn();
            if(point2)
                pointlight2.turnAmbientOn();
        }
    }

    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        if (pointlight1.diffuseOn > 0 || pointlight2.diffuseOn > 0) {
            if (point1)
                pointlight1.turnDiffuseOff();
            if (point2)
                pointlight2.turnDiffuseOff();
        }
        else {
            if (point1)
                pointlight1.turnDiffuseOn();
            if (point2)
                pointlight2.turnDiffuseOn();
        }
    }

    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        if (pointlight1.specularOn > 0 || pointlight2.specularOn > 0) {
            if (point1)
                pointlight1.turnSpecularOff();
            if (point2)
                pointlight2.turnSpecularOff();
        }
        else {
            if (point1)
                pointlight1.turnSpecularOn();
            if (point2)
                pointlight2.turnSpecularOn();
        }
    }
}

//glfw whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //make sure the viewport matches the new window dimensions; note that width and height will be significantly larger than specified on retina displays.
    float aspectRatio = 4.0f / 3.0f;
    int viewWidth, viewHeight;

    if (width / (float)height > aspectRatio) {
        //Window is too wide, fit height and adjust width
        viewHeight = height;
        viewWidth = (int)(height * aspectRatio);
    }
    else {
        //Window is too tall, fit width and adjust height
        viewWidth = width;
        viewHeight = (int)(width / aspectRatio);
    }

    //Center the viewport
    int xOffset = (width - viewWidth) / 2;
    int yOffset = (height - viewHeight) / 2;

    glViewport(xOffset, yOffset, viewWidth, viewHeight);
    //glViewport(0, 0, width, height);
}

//glfw whenever the mouse moves, this callback is called
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
    float yoffset = lastY - ypos;       //reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

//glfw whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}