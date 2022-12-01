
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <learnopengl/filesystem.h>
#include "shader.h"
#include "camera.h"

#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>

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


int main() {

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

    Shader lightingShader("../2.1.point.vs", "../2.1.point.fs", "../2.1.point.gs");
    // Shader lightingShader("../2.1.basic_lighting.vs", "../2.1.basic_lighting.fs");
     //Shader lightCubeShader("2.1.light_cube.vs", "2.1.light_cube.fs");

    vector<vec3> puntos, normales;
    
    //Generar PUNTOS
    size_t number_of_points = 3;
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
        for (float x = xMenor; x < xMayor; x += step) {
           for (int t = 0; t < points_per_circ; ++t) {
               float angle = 2.f * 3.141592654f * t / points_per_circ;
               float y_off = 1.0f * glm::cos(angle);
               float z_off = 1.0f * glm::sin(angle);
 
               puntos.emplace_back(vec3(x, puntosA[i].y + y_off, puntosA[i].z + z_off));
               normales.emplace_back(normalize(vec3(x, puntosA[i].y + y_off, puntosA[i].z + z_off)));
           }
       }

        //generar cilindro de y a y, empezando en nuevo x
        for (float y = yMenor; y < yMayor; y += step) {
           for (int t = 0; t < points_per_circ; ++t) {
               float angle = 2.f * 3.141592654f * t / points_per_circ;
               float x_off = 1.0f * glm::cos(angle);
               float z_off = 1.0f * glm::sin(angle);
 
               puntos.emplace_back(vec3(puntosA[i + 1].x + x_off, y, puntosA[i].z + z_off));
               normales.emplace_back(normalize(vec3(puntosA[i + 1].x + x_off, y, puntosA[i].z + z_off)));
           }
       }

        //generar cilindro de z a z, empezando con nuevo 'x' e 'y'
        for (float z = zMenor; z < zMayor; z += step) {
           for (int t = 0; t < points_per_circ; ++t) {
               float angle = 2.f * 3.141592654f * t / points_per_circ;
               float x_off = 1.0f * glm::cos(angle);
               float y_off = 1.0f * glm::sin(angle);
 
               puntos.emplace_back(vec3(puntosA[i + 1].x + x_off, puntosA[i + 1].y + y_off, z));
               normales.emplace_back(normalize(vec3(puntosA[i + 1].x + x_off, puntosA[i + 1].y + y_off, z)));
           }
       }
    }

    //Triangular
    vector<vector<vec3>> triangulos;
    vector<vec3> temp;
    for(int i = 0; i<1000; i++){
        temp.clear();
        temp.push_back(puntos[i]);
        temp.push_back(puntos[i+1]);
        temp.push_back(puntos[i+100]);
        triangulos.push_back(temp);
        temp.clear();
        temp.push_back(puntos[i+1]);
        temp.push_back(puntos[i+100]);
        temp.push_back(puntos[i+101]);
        triangulos.push_back(temp);
    }

    // first, configure the cube's VAO (and VBO)
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO[2], cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(cubeVAO);
    // position attribute
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, puntos.size() * sizeof(vec3), puntos.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, normales.size() * sizeof(vec3), normales.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
    glPointSize(2);

    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_POINTS, 0, puntos.size());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(2, VBO);

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