
/*
model maps from an object's local coordinate space into world space, 
view from world space to camera space, 
projection from camera to screen.
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>


#include "Line.h"
#include "Camera.h"
#include "GenerateTerrain.h"
#include "Vertex.h"
#include "YAndNormal.h"

#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// vertex shader
const char* getVertexShaderSource()
{
    return "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;"
    "layout (location = 1) in vec3 aNorm;"
    "layout (location = 2) in vec2 aTex;"
    "uniform mat4 model;"
    "uniform mat4 view;"
    "uniform mat4 projection;"
    "out vec3 normals;"
    "out vec3 position;"
    "void main()"
    "{"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0f);"
    "   position = vec3((model * vec4(aPos, 1.0f)));"
    "   normals = aNorm;"
    "}";
}

// fragment shader
const char* getFragmentShaderSource()
{
    return "#version 330 core\n"
    "in vec3 position;"
    "in vec3 normals;"
    "uniform sampler2D tex_rock;"
    "uniform sampler2D tex_sand;"
    "uniform vec4 vertexColour;"
    "out vec4 FragColour;"
    "void main()"
    "{"
    "   float n = (normals.y * 0.5) + 0.5;"
    "   vec4 FragColourSand = texture(tex_sand, position.xz/8) * (1-n);"
    "   vec4 FragColourRock = texture(tex_rock, position.xz/8) * (n);"
    "   FragColour = FragColourSand + FragColourRock;"
    "}";
}


GLuint loadTexture(const char *filename)
{
    // create and bind textures
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // load textures with dimension data
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // upload the texture to the GPU
    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}

int compileAndLinkShaders()
{
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = getVertexShaderSource();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = getFragmentShaderSource();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;

    return -1;
}

int main(int argc, char*argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Marching Cubes", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // compile and link shaders
    int shaderProgram = compileAndLinkShaders();

    // setting perspective doesnt need to be repeated
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // camera object
	Camera camera(shaderProgram, SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.5f, 40.0f), 60.0f, 0.1f, 100.0f);

    // coordinate lines
    Line line1(glm::vec3(0,0,0), glm::vec3(1,0,0), glm::vec3(1,0,0), shaderProgram); // red x coordinate
    Line line2(glm::vec3(0,0,0), glm::vec3(0,1,0), glm::vec3(0,1,0), shaderProgram); // green y coordinate
    Line line3(glm::vec3(0,0,0), glm::vec3(0,0,1), glm::vec3(0,0,1), shaderProgram); // blue z coordinate

    // enable depth test
    glEnable(GL_DEPTH_TEST); // hidden surface removal
    glDepthFunc(GL_LESS);

    // Define terrain parameters
    int grid_size = 100;
    float iso_surface_level = 0.0f;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(1, 1000); 
    int seed = distribution(gen); 

    // FOR GETTING XZ RELATED Y WE CAN JUST HAVE SMALL CHUNKS , CHECK WHAT CHUNK WERE IN AND LOOP THROUGH VERTICES OF THAT CHUNK

    // Create the terrain generator
    GenerateTerrain terrain(grid_size, iso_surface_level, glm::vec3(0.0f,0.0f,0.0f), seed, shaderProgram);
    terrain.generateChunkTerrain();
    terrain.setupBuffers();

    // Create the terrain generator
    GenerateTerrain terrain_testing(grid_size, iso_surface_level, glm::vec3(grid_size,0.0f,0.0f), seed, shaderProgram);
    terrain_testing.generateChunkTerrain();
    terrain_testing.setupBuffers();

    // textures
    GLuint texSandUni = glGetUniformLocation(shaderProgram, "tex_sand");
    GLuint texRockUni = glGetUniformLocation(shaderProgram, "tex_rock");

    // Entering Main Loop
    while(!glfwWindowShouldClose(window))
    {   
        // input
        camera.processCameraInputs(window);

        // each frame, reset color and depth buffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        // set textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, loadTexture("assets/yello.jpg"));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, loadTexture("assets/blue.jpg"));

        glUniform1i(texSandUni, 0);
        glUniform1i(texRockUni, 1);

        // matrix transformations
        camera.updateCameraMatrix();

        // drawing
        line1.drawLine();
        line2.drawLine();
        line3.drawLine();

        terrain.drawChunk();
        terrain_testing.drawChunk();

        // end frame
        glfwSwapBuffers(window);
        
        // detect inputs
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }
    }   
    
    // shutdown GLFW
    glfwTerminate();
    
	return 0;
}
