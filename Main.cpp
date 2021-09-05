
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Cubesphere.h"
#include "Skybox.h"
#include <iostream>

const int subdivision = 6;

const int useCubeSphere = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastY = SCR_HEIGHT / 2.0f;
float lastX = SCR_WIDTH / 2.0f;
bool firstMouse = true;
bool clickStarted = false;

// Time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Lighting
glm::vec3 lightPos(3.0f, 0.5f, 1.5f);

int main()
{
    // glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader cubesphereShader("Shader.vs", "Shader.fs");
    Shader skyboxShader("skyboxShader.vs", "skyboxShader.fs");

    Cubesphere cubesphere = Cubesphere(subdivision, &cubesphereShader, useCubeSphere);
    int numberOfVertices = cubesphere.numberOfVerticesToDraw;
    
    std::vector<std::string> textures_faces{ "px.png", "nx.png", "py.png", "ny.png", "pz.png", "nz.png" };
    std::vector<std::string> height_faces{ "heightMap-px.png", "heightMap-nx.png", "heightMap-py.png", "heightMap-ny.png", "heightMap-pz.png", "heightMap-nz.png" };
    std::vector<std::string> specular_faces{ "specularMap-px.png", "specularMap-nx.png", "specularMap-py.png", "specularMap-ny.png", "specularMap-pz.png", "specularMap-nz.png" };

    if (useCubeSphere) {
        cubesphere.initEarthTextureCubeMap(textures_faces);
        cubesphere.initEarthHeightTextureCubeMap(height_faces);
        cubesphere.initEarthSpecularTextureCubeMap(specular_faces);
    }
    else {
        cubesphere.initEarthTexture2D();
        cubesphere.initEarthHeightTexture();
        cubesphere.initEarthSpecularTexture();
    }
    
    
  

    Skybox skybox = Skybox(&skyboxShader);
    std::vector<std::string> skybox_faces{ "skybox-px.png", "skybox-nx.png", "skybox-py.png", "skybox-ny.png", "skybox-pz.png", "skybox-nz.png" };
    skybox.initCubemapTexture(skybox_faces);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // light properties
    cubesphereShader.use();
    cubesphereShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    cubesphereShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
    cubesphereShader.setVec3("light.specular", 0.4f, 0.4f, 0.4f);
    cubesphereShader.setVec3("light.position", lightPos);
    cubesphereShader.setFloat("light.constant", 1.0f);
    cubesphereShader.setFloat("light.linear", 0.0014f);
    cubesphereShader.setFloat("light.quadratic", 0.000007f);


    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubesphereShader.use();

        cubesphereShader.setVec3("viewPos", camera.Position);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        cubesphereShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        cubesphereShader.setMat4("view", view);

        // render Earth
        glBindVertexArray(cubesphere.VAO);
        glm::mat4 model = glm::mat4(1.0f);
        if (useCubeSphere) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubesphere.cubemapTexture);
            model = glm::rotate(model, glm::radians(150.0f), glm::vec3(0.0f,1.0f,0.0f));
        }
        else {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cubesphere.texture2D);
            model = glm::rotate(model, glm::radians(-60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        } 
        
        cubesphereShader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, numberOfVertices, GL_UNSIGNED_INT, 0);


        // draw skybox
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when cube's depth is 1.0 (like the depth buffer max)
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        glBindVertexArray(skybox.VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
        

        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &(cubesphere.VAO));
    glDeleteBuffers(1, &(cubesphere.VBO));
    glDeleteVertexArrays(1, &(skybox.VAO));
    glDeleteBuffers(1, &(skybox.VBO));

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    /*if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);*/
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (clickStarted) {
        float xoffset, yoffset;
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        xoffset = xpos - lastX;
        yoffset = ypos - lastY;
        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    } 
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            clickStarted = true;
            firstMouse = true;
        }
        else {
            clickStarted = false;
        }
    }
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}



