
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <learnopengl/filesystem.h>
#include "shader.h"
#include "camera.h"
#include "Triangle.h"

#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <ctime>

using namespace std;
using namespace glm;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


vector<float> vertices;
vector<Triangle*> triangles;
unsigned int quadVAO = 0;
unsigned int quadVBO;

void renderQuad(){
    if (quadVAO == 0)
    {
      float* quadVertices =  &vertices[0];
      // configure plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size()/14);
    glBindVertexArray(0);
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        //std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


int main() {
    // srand(time(NULL));
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    Shader shader("../4.normal_mapping.vs", "../4.normal_mapping.fs");

    // load textures
    // -------------
    unsigned int diffuseMap = loadTexture("../resources/textures/brickwall.jpg");
    unsigned int normalMap  = loadTexture("../resources/textures/brickwall_normal.jpg");

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("diffuseMap", 0);
    shader.setInt("normalMap", 1);

    glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

    vector<vec3> puntos, normales;
    
    //Generar PUNTOS
    size_t number_of_points = 2;
    float points_per_circ = 100.f;
    float step = 0.2;
    std::vector<vec3> puntosA(number_of_points);

    int min = -10, max = 10;
    for(int i = 0; i < number_of_points; i++){
        float tempX, tempY, tempZ;
        tempX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
        tempY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
        tempZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));

        puntosA[i] = vec3(tempX + min, tempY + min, tempZ + min);
    }

    //Mostrar Puntos
    for(int i = 0; i < number_of_points; i++){
        std::cout<<puntosA[i].x<<' '<<puntosA[i].y<<' '<<puntosA[i].z<<'\n';
    }
  

    float x, y ,z;
    float xMenor, xMayor, yMenor, yMayor, zMenor, zMayor;

    //Generar rutas
    for(int i = 0; i < number_of_points - 1; i++){
        xMenor = std::min(puntosA[i].x, puntosA[i+1].x);
        xMayor = std::max(puntosA[i].x, puntosA[i+1].x);
        
        yMenor = std::min(puntosA[i].y, puntosA[i+1].y);
        yMayor = std::max(puntosA[i].y, puntosA[i+1].y);
        
        zMenor = std::min(puntosA[i].z, puntosA[i+1].z); 
        zMayor = std::max(puntosA[i].z, puntosA[i+1].z);

        //generar cilindro de x a x
        long long capas_x, capas_y, capas_z;
        capas_x = capas_y = capas_z = 0;

        for (float x = xMenor; x < xMayor; x += step) {
           for (int t = 0; t <= points_per_circ; ++t) {
               float angle = 2.f * 3.141592654f * t / points_per_circ;
               float y_off = 1.0f * glm::cos(angle);
               float z_off = 1.0f * glm::sin(angle);
 
               puntos.emplace_back(vec3(x, puntosA[i].y + y_off, puntosA[i].z + z_off));
           }
           capas_x++;
       }

        //generar cilindro de y a y, empezando en nuevo x
        for (float y = yMenor; y < yMayor; y += step) {
           for (int t = 0; t <= points_per_circ; ++t) {
               float angle = 2.f * 3.141592654f * t / points_per_circ;
               float x_off = 1.0f * glm::cos(angle);
               float z_off = 1.0f * glm::sin(angle);
 
               puntos.emplace_back(vec3(puntosA[i + 1].x + x_off, y, puntosA[i].z + z_off));
           }
           capas_y++;
       }

        //generar cilindro de z a z, empezando con nuevo 'x' e 'y'
        for (float z = zMenor; z < zMayor; z += step) {
           for (int t = 0; t <= points_per_circ; ++t) {
               float angle = 2.f * 3.141592654f * t / points_per_circ;
               float x_off = 1.0f * glm::cos(angle);
               float y_off = 1.0f * glm::sin(angle);
 
               puntos.emplace_back(vec3(puntosA[i + 1].x + x_off, puntosA[i + 1].y + y_off, z));
           }
           capas_z++;
       }
    }

    //Triangular
    for(int i = 0; i < puntos.size() - 100 - 1; i++){
        float dist = glm::distance(puntos[i+1], puntos[i+100]); 
        if (dist > 0.5 || dist < 0.07) continue;
        // cout << "distance = " << glm::distance(puntos[i+1], puntos[i+100]) << endl;
        triangles.push_back(new Triangle(puntos[i], puntos[i + 1], puntos[i + 100]));
        triangles.push_back(new Triangle(puntos[i+1], puntos[i+100], puntos[i+101]));
    }

    for(auto& triangle: triangles) {
        triangle->insertObject(vertices);
    }

    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // render normal-mapped quad
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);


        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        shader.setMat4("model", model);
        renderQuad();

        // render the cube
        // glBindVertexArray(cubeVAO);
        // glDrawArrays(GL_POINTS, 0, puntos.size());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &quadVAO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
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