#include <iostream>
#include <vector>
#include <fstream>
#include <list>
#include <cstring>

#define GLEW_STATIC 1  
#include <GL/glew.h> 

#include <GLFW/glfw3.h>


#include <glm/glm.hpp>  
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/common.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

///////////////////////////////////////////////////////////
//NICOLAS TOWA KAMGNE ID:40154685
///////////////////////////////////////////////////////////
using namespace glm;
using namespace std;

GLuint shader;
GLuint loadTexture(const char* filename);
// Function to import a shader from a .glsl shader file, return the shader content as a string
string shaderImport(const char* fileName)
{
    //Verifing if the shader file is accessible
    ifstream meInput(fileName);
    if (!meInput.good())
    {
        cout << "The shader file with the name " << fileName << "was not found!";
        exit(1);
    }

    string shaderCode = "";

    //Iterating over the shader file until reaching the last line of code
    while (meInput.peek() != EOF)
    {
        shaderCode += meInput.get();
    }
    //Returning the string
    return shaderCode;
}

void AddShader(GLuint Program, const char* shaderCode, GLenum TypeofShader)
{
    GLuint shadeR = glCreateShader(TypeofShader);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    glShaderSource(shadeR, 1, theCode, codeLength);
    glCompileShader(shadeR);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv(shadeR, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        glGetShaderInfoLog(shadeR, sizeof(eLog), NULL, eLog);
        cout << "Error compiling " << TypeofShader << " shader: \n" << eLog;
        return;
    }

    glAttachShader(Program, shadeR);

}

// 
//This is the function that creates a program, reads 2 shaders and compiles and adds them to the program. Lastly, it also links and validates the program.
void CreateShaders()
{
    shader = glCreateProgram();

    if (!shader)
    {
        cout << "An error occured while creating the shader program";
        return;
    }

    string vertexShader = shaderImport("../shaders/VertexShader.glsl");
    string fragmentShader = shaderImport("../shaders/FragmentShader.glsl");

    AddShader(shader, vertexShader.c_str(), GL_VERTEX_SHADER);
    AddShader(shader, fragmentShader.c_str(), GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &result);

    if (!result)
    {
        cout << "Error Linking program";
        return;
    }

    glValidateProgram(shader);

    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

    if (!result)
    {
        cout << "Error Validating program";
        return;
    }

}

//create lines for the grid
int createLine()
{
    vec3 vertexArray[] = {
        {vec3(0.0f, 0.0f, 50.0f)}, //vec2(0.0f,0.0f)},//gives error
        {vec3(0.0f, 0.0f,-50.0f)}, //vec2(0.0f,0.0f)},
    };

    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        sizeof(vec3),        // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) + sizeof(vec2), (void*)(2 * sizeof(vec3)));
    glEnableVertexAttribArray(1);

    return vertexBufferObject;
}//////////////////////////////////////////////////////
// Sphere
struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
};

GLuint mVAO, mVBO, numOfVertices;



int createUnitCube() //Creating the unit cube that will help us draw the arm and racket (12 triangles are needed since a cube as 6 faces and a face is composed of 2 triangles)
{
    vec3 vertexArray[] = {
        //backface
        vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f),
        vec3(1.0f, 1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f),
        vec3(1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f),

        vec3(1.0f, 1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f),
        vec3(-1.0f, -1.0f, -1.0f),vec3(0.0f, 0.0f, -1.0f),
        vec3(-1.0f, 1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f),
        //frontface
        vec3(-1.0f, -1.0f, 1.0f),vec3(0.0f, 0.0f, 1.0f),
        vec3(1.0f, -1.0f, 1.0f),vec3(0.0f, 0.0f, 1.0f),
        vec3(1.0f, 1.0f, 1.0f),vec3(0.0f, 0.0f, 1.0f),

        vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f),
        vec3(-1.0f, 1.0f, 1.0f),vec3(0.0f, 0.0f, 1.0f),
        vec3(-1.0f, -1.0f, 1.0),vec3(0.0f, 0.0f, 1.0f),
        //leftface
        vec3(-1.0f, 1.0f, 1.0f),  vec3(-1.0f, 0.0f, 0.0f),
        vec3(-1.0f, 1.0f, -1.0f), vec3(-1.0f, 0.0f, 0.0f),
        vec3(-1.0f, -1.0f, -1.0f),vec3(-1.0f, 0.0f, 0.0f),

        vec3(-1.0f, -1.0f, -1.0f),vec3(-1.0f, 0.0f, 0.0f),
        vec3(-1.0f, -1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f),
        vec3(-1.0f, 1.0f, 1.0f),  vec3(-1.0f, 0.0f, 0.0f),
        //right face
        vec3(1.0f, 1.0f, 1.0f),  vec3(1.0f, 0.0f, 0.0f),
        vec3(1.0f, -1.0f, -1.0f),vec3(1.0f, 0.0f, 0.0f),
        vec3(1.0f, 1.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f),

        vec3(1.0f, -1.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f),
        vec3(1.0f, 1.0f, 1.0f),   vec3(1.0f, 0.0f, 0.0f),
        vec3(1.0f, -1.0f, 1.0f),  vec3(1.0f, 0.0f, 0.0f),
        //bottom face
        vec3(-1.0f, -1.0f, -1.0f),vec3(0.0f, -1.0f, 0.0f),
        vec3(1.0f, -1.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f),
        vec3(1.0f, -1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f),

        vec3(1.0f, -1.0f, 1.0f),  vec3(0.0f, -1.0f, 0.0f),
        vec3(-1.0f, -1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f),
        vec3(-1.0f, -1.0f, -1.0f),vec3(0.0f, -1.0f, 0.0f),
        //top face
        vec3(-1.0f, 1.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f),
        vec3(1.0f, 1.0f, 1.0f) ,  vec3(0.0f, 1.0f, 0.0f),
        vec3(1.0f, 1.0f, -1.0f),  vec3(0.0f, 1.0f, 0.0f),

        vec3(1.0f, 1.0f, 1.0f) , vec3(0.0f,1.0f,0.0f),
        vec3(-1.0f, 1.0f, -1.0f),vec3(0.0f,1.0f,0.0f),
        vec3(-1.0f, 1.0f, 1.0f), vec3(0.0f,1.0f,0.0f),
    };


    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        sizeof(Vertex), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);
    //vertex normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));
    glEnableVertexAttribArray(1);

    //vertex color
   // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(vec3)));
    //glEnableVertexAttribArray(2);

    return vertexBufferObject;
}
//NEW CODE

int createTrapezoid(float x, float y, float z) //Creating the unit cube that will help us draw the arm and racket (12 triangles are needed since a cube as 6 faces and a face is composed of 2 triangles)
{
    float sizeX = x;
    float sizeZ = y;
    float height = z;
    vec3 vertexArray[] = {
        //backface
        vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f),
        vec3(sizeX, height, -sizeZ), vec3(0.0f, 0.0f, -1.0f),
        vec3(1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f),

        vec3(sizeX, height, -sizeZ), vec3(0.0f, 0.0f, -1.0f),
        vec3(-1.0f, -1.0f, -1.0f),vec3(0.0f, 0.0f, -1.0f),
        vec3(-sizeX, height, -sizeZ), vec3(0.0f, 0.0f, -1.0f),
        //frontface
        vec3(-1.0f, -1.0f, 1.0f),vec3(0.0f, 0.0f, 1.0f),
        vec3(1.0f, -1.0f, 1.0f),vec3(0.0f, 0.0f, 1.0f),
        vec3(sizeX, height, sizeZ),vec3(0.0f, 0.0f, 1.0f),

        vec3(sizeX, height, sizeZ), vec3(0.0f, 0.0f, 1.0f),
        vec3(-sizeX, height, sizeZ),vec3(0.0f, 0.0f, 1.0f),
        vec3(-1.0f, -1.0f, 1.0),vec3(0.0f, 0.0f, 1.0f),
        //leftface
        vec3(-sizeX, height, sizeZ),  vec3(-1.0f, 0.0f, 0.0f),
        vec3(-sizeX, height, -sizeZ), vec3(-1.0f, 0.0f, 0.0f),
        vec3(-1.0f, -1.0f, -1.0f),vec3(-1.0f, 0.0f, 0.0f),

        vec3(-1.0f, -1.0f, -1.0f),vec3(-1.0f, 0.0f, 0.0f),
        vec3(-1.0f, -1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f),
        vec3(-sizeX, height, sizeZ),  vec3(-1.0f, 0.0f, 0.0f),
        //right face
        vec3(sizeX, height, sizeZ),  vec3(1.0f, 0.0f, 0.0f),
        vec3(1.0f, -1.0f, -1.0f),vec3(1.0f, 0.0f, 0.0f),
        vec3(sizeX, height, -sizeZ), vec3(1.0f, 0.0f, 0.0f),

        vec3(1.0f, -1.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f),
        vec3(sizeX, height, sizeZ),   vec3(1.0f, 0.0f, 0.0f),
        vec3(1.0f, -1.0f, 1.0f),  vec3(1.0f, 0.0f, 0.0f),
        //bottom face
        vec3(-1.0f, -1.0f, -1.0f),vec3(0.0f, -1.0f, 0.0f),
        vec3(1.0f, -1.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f),
        vec3(1.0f, -1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f),

        vec3(1.0f, -1.0f, 1.0f),  vec3(0.0f, -1.0f, 0.0f),
        vec3(-1.0f, -1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f),
        vec3(-1.0f, -1.0f, -1.0f),vec3(0.0f, -1.0f, 0.0f),
        //top face
        vec3(-sizeX, height, -sizeZ), vec3(0.0f, 1.0f, 0.0f),
        vec3(sizeX, height, sizeZ) ,  vec3(0.0f, 1.0f, 0.0f),
        vec3(sizeX, height, -sizeZ),  vec3(0.0f, 1.0f, 0.0f),

        vec3(sizeX, height, sizeZ) , vec3(0.0f,1.0f,0.0f),
        vec3(-sizeX, height, -sizeZ),vec3(0.0f,1.0f,0.0f),
        vec3(-sizeX, height, sizeZ), vec3(0.0f,1.0f,0.0f),
    };


    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        sizeof(Vertex), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);
    //vertex normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));
    glEnableVertexAttribArray(1);

    //vertex color
   // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(vec3)));
    //glEnableVertexAttribArray(2);

    return vertexBufferObject;
}
float getRandomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    // Initialize GLFW and OpenGL version
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Assignment 1 - Tennis Racket - 40154685", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    

     // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    //New
    GLuint clayTextureID = loadTexture("assets/textures/clay.jpeg");
    // Background color
    glClearColor(0.6f, 0.0f, 0.6f, 1.0f); //orang yellowish

    // Shader linking and compilation
    CreateShaders();

    glUseProgram(shader);

    // Camera parameters for view transform
    vec3 cameraPosition(35.0f, 20.0f, 35.0f); //represents the position of the camera in 3D space
    vec3 cameraLookAt(-10.0f, -5.0f, -10.0f); //represents the target or the direction in which the camera is oriented
    vec3 cameraUp(0.0f, 1.0f, 0.0f); // defines the upward direction for the camera. It specifies the orientation of the camera's top or the "up" direction

    // Other camera parameters
    float cameraHAngle = 135.0f; //represents the horizontal angle of the camera
    float cameraVAngle = -20.0f; //represents the vertical angle of the camera

    // Creation of projection matrix
    mat4 projectionMatrix = glm::perspective(70.0f, //represents the field of view (FOV) angle in degrees. It defines the extent of the viewing frustum vertically. At 70 degrees the field of view is pretty wide
        1024.0f / 768.0f, //  ratio between the width and height of the viewport or the screen, 4:3 ratio
        0.01f, 200.0f);   // near and far clipping plane distance, any objects closer than 0.01 units or farther than 200 units from the camera will be clipped and not rendered

    GLuint projectionMatrixLocation = glGetUniformLocation(shader, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]); // Set value of projectionMatrix in the vertex shader

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
        cameraPosition + cameraLookAt,  // center
        cameraUp); // up

    GLuint viewMatrixLocation = glGetUniformLocation(shader, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]); // Set value of viewMatrix in the vertex shader

    // upload geometry to GPU
    int vao1 = createLine();
    int unitCubeVAO = createUnitCube();
    int trapezoidvao = createTrapezoid(0.5, 0.25, 0.25f);
    //Coordinates for the ball, change tx ty tz so that the balls don't overlap 
    float  tx = -8.5f, ty = 10.0f, tz = 10.0f, radx = 0.0f, rady = 0.0f, sx = 1.0f, sy = 1.0f, sz = 1.0f, rad = 0.0f, dt = 0.0f, dx, dy, theta, phi;

    vec3 lightPos(0.0f, 0.0f, 0.0f);
    GLuint lightLocation = glGetUniformLocation(shader, "lightPos");
    glUniform3fv(lightLocation, 1, &lightPos[0]);
    // Frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    int lastMouseRightState = GLFW_RELEASE;
    int lastMouseMiddleState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    // Racket Model Important Parameters
    mat4 initialPosition = mat4(1.0);
    mat4 initialScale = mat4(1.0);
    mat4 initialRotation = mat4(1.0);
    mat4 initialRotation1 = mat4(1.0);
    mat4 initialRotation2 = mat4(1.0);
    mat4 initialRotation3 = mat4(1.0);
    mat4 initialRotation4 = mat4(1.0);
    mat4 initialRotation5 = mat4(1.0);
    // World Properties
    mat4 worldRotation = mat4(1.0f);
    GLuint worldOrientationMatrixLocation = glGetUniformLocation(shader, "worldOrientationMatrix");
    mat4 worldOrientationMatrix = mat4(1.0);
    //FragColor = vec4(ambientColor+diffuseColor+specularColor,1.0f);
    //Input Tracking Variables that we will need later on
    bool RandomGridLocation = false;
    int randomGridLocationX = 0;
    int randomGridLocationZ = 0;
    int rX = 0;
    int rZ = 0;
    vec3 targetPoint = vec3(0.0f);
    vec3 directionVector = vec3(0.0f);

    // Enable Backface culling
    //glEnable(GL_CULL_FACE);

    //Enable Depth Test
    glEnable(GL_DEPTH_TEST);

    GLuint Rendering = GL_TRIANGLES; //Variable to keep track of Racket's rendering mode

    class Shark {
    public:
        float txPos = 0.0f;
        float tyPos = 0.0f;
        float tzPos = 0.0f;

        float getRandomFloat(float min, float max) {
            return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
        }
        // Constructor
        Shark(float x) {
            // Initialize member variable
            xPos = getRandomFloat(-30.0f, 30.0f);
            yPos = getRandomFloat(-30.0f, 30.0f);
            zPos = getRandomFloat(-30.0f, 30.0f);
            txPos = xPos+0.9f;
            tyPos = yPos + 0.9f;
            tzPos = zPos + 0.9f;
            taille = 0.005625f*x*3;
            sharkPos = vec3(xPos, yPos, zPos);
            targetPos = vec3(txPos, tyPos, tzPos);
            travel = targetPos - sharkPos;
            travel = glm::normalize(travel);
            float a = getRandomFloat(0.0f, 1.0f);
            float b = getRandomFloat(0.0f, 1.0f);
            float c = getRandomFloat(0.0f, 1.0f);
            color = vec3(a, b, c);
            color1 = vec3(b, a, b);
            color2 = vec3(c, b, a);
            coin = getRandomFloat(-1.0f, 1.0f);

        }
        
        void draw() {
            if (coin < 0.0f) {
                update();
                initialRotation = rotate(mat4(1.0f), glm::radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
                initialScale = scale(mat4(1.0f), vec3(taille, taille, taille));
                initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(time / (1.25f)) * 10), vec3(1.0f, 0.0f, 0.0f));
                initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(time / (1.50f)) * 9), vec3(1.0f, 0.0f, 0.0f));
                initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(time / (1.75f)) * 9), vec3(1.0f, 0.0f, 0.0f));
                initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(time / (2.0f)) * 8), vec3(1.0f, 0.0f, 0.0f));

                GLuint Rendering = GL_TRIANGLES;
                int trapezoidvao1 = createTrapezoid(0.40, 0.85, 1.25f);
                glBindVertexArray(trapezoidvao1);
                vec3 rightVector = glm::cross(vec3(0.0f, 1.0f, 0.0f), travel);
                rightVector = glm::normalize(rightVector);
                vec3 realUp = glm::cross(travel, rightVector);
                mat4 trap = initialPosition * translate(mat4(1.0f), sharkPos)
                    *  targetRotation *travelRotation * initialRotation * initialScale * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
                GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                GLuint cc = glGetUniformLocation(shader, "customColor");
                vec3 customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trap[0][0]);
                glDrawArrays(Rendering, 0, 36);
                mat4 trapeze = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.75f, 2.75f, 4.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);
                glDrawArrays(Rendering, 0, 36);
                mat4 trapezeChild = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
                glBindVertexArray(unitCubeVAO);

                mat4 br1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -10.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.5f, 5.75f, 5.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);
                mat4 cubechild = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -10.0f, 0.0f));

                glBindVertexArray(trapezoidvao1);


                mat4 wing = cubechild * translate(mat4(1.0f), vec3(0.0f, 0.0f, 7.5f)) * rotate(glm::mat4(1.0f), glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(1.75f, 3.5f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &wing[0][0]);
                glDrawArrays(Rendering, 0, 36);

                wing = cubechild * translate(mat4(1.0f), vec3(0.0f, 0.0f, -7.5f)) * rotate(glm::mat4(1.0f), glm::radians(105.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(1.75f, 3.5f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &wing[0][0]);
                glDrawArrays(Rendering, 0, 36);

                glBindVertexArray(unitCubeVAO);

                br1 = cubechild * initialRotation2 * translate(mat4(1.0f), vec3(0.0f, -12.5f, 0.0f)) * scale(mat4(1.0f), vec3(5.5f, 5.75f, 4.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild1 = cubechild * initialRotation2 * translate(mat4(1.0f), vec3(0.0f, -12.5f, 0.0f));
                int trapezoidvao2 = createTrapezoid(0.40, 0.85, 0.1f);
                glBindVertexArray(trapezoidvao2);

                br1 = trapezeChild1 * initialRotation3 * translate(mat4(1.0f), vec3(0.0f, -11.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(5.25f, 4.75f, 3.5f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild2 = trapezeChild1 * initialRotation3 * translate(mat4(1.0f), vec3(0.0f, -12.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

                br1 = trapezeChild2 * initialRotation4 * translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(5.25f, 2.5f, 1.5f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);

            }
            else if (coin == 0.0f) { //Very unlikely
                update();
                initialRotation = rotate(mat4(1.0f), glm::radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
                initialScale = scale(mat4(1.0f), vec3(taille, taille, taille));
                initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(time / (1.25f)) * 10), vec3(1.0f, 0.0f, 0.0f));
                initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(time / (1.50f)) * 9), vec3(1.0f, 0.0f, 0.0f));
                initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(time / (1.75f)) * 9), vec3(1.0f, 0.0f, 0.0f));
                initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(time / (2.0f)) * 8), vec3(1.0f, 0.0f, 0.0f));

                GLuint Rendering = GL_TRIANGLES;
                int trapezoidvao1 = createTrapezoid(0.40, 0.85, 1.25f);
                glBindVertexArray(trapezoidvao1);
                vec3 rightVector = glm::cross(vec3(0.0f, 1.0f, 0.0f), travel);
                rightVector = glm::normalize(rightVector);
                vec3 realUp = glm::cross(travel, rightVector);
                mat4 trap = initialPosition * translate(mat4(1.0f), sharkPos)
                    * travelRotation * targetRotation * initialRotation * worldRotation * initialScale * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
                GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                GLuint cc = glGetUniformLocation(shader, "customColor");
                vec3 customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trap[0][0]);
                glDrawArrays(Rendering, 0, 36);
                mat4 trapeze = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.75f, 2.75f, 4.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);
                glDrawArrays(Rendering, 0, 36);
                mat4 trapezeChild = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
                glBindVertexArray(unitCubeVAO);

                mat4 br1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -10.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.5f, 5.75f, 5.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);
                mat4 cubechild = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -10.0f, 0.0f));

                glBindVertexArray(trapezoidvao1);


                mat4 wing = cubechild * translate(mat4(1.0f), vec3(0.0f, 0.0f, 7.5f)) * rotate(glm::mat4(1.0f), glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(1.75f, 3.5f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &wing[0][0]);
                glDrawArrays(Rendering, 0, 36);

                wing = cubechild * translate(mat4(1.0f), vec3(0.0f, 0.0f, -7.5f)) * rotate(glm::mat4(1.0f), glm::radians(105.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(1.75f, 3.5f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &wing[0][0]);
                glDrawArrays(Rendering, 0, 36);

                glBindVertexArray(unitCubeVAO);

                br1 = cubechild * initialRotation2 * translate(mat4(1.0f), vec3(0.0f, -12.5f, 0.0f)) * scale(mat4(1.0f), vec3(5.5f, 5.75f, 4.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild1 = cubechild * initialRotation2 * translate(mat4(1.0f), vec3(0.0f, -12.5f, 0.0f));
                int trapezoidvao2 = createTrapezoid(0.40, 0.85, 0.1f);
                glBindVertexArray(trapezoidvao2);

                br1 = trapezeChild1 * initialRotation3 * translate(mat4(1.0f), vec3(0.0f, -11.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(5.25f, 4.75f, 3.5f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild2 = trapezeChild1 * initialRotation3 * translate(mat4(1.0f), vec3(0.0f, -12.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

                br1 = trapezeChild2 * initialRotation4 * translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(5.25f, 2.5f, 1.5f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);
            }
            else {
                update();
                initialRotation = rotate(mat4(1.0f), glm::radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
                initialScale = scale(mat4(1.0f), vec3(taille, taille, taille));
                initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(time / (1.25f)) * 10), vec3(1.0f, 0.0f, 0.0f));
                initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(time / (1.50f)) * 9), vec3(1.0f, 0.0f, 0.0f));
                initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(time / (1.75f)) * 9), vec3(1.0f, 0.0f, 0.0f));
                initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(time / (2.0f)) * 8), vec3(1.0f, 0.0f, 0.0f));

                GLuint Rendering = GL_TRIANGLES;
                int trapezoidvao1 = createTrapezoid(0.40, 0.85, 1.25f);
                glBindVertexArray(trapezoidvao1);
                vec3 rightVector = glm::cross(vec3(0.0f, 1.0f, 0.0f), travel);
                rightVector = glm::normalize(rightVector);
                vec3 realUp = glm::cross(travel, rightVector);
                mat4 trap = initialPosition * translate(mat4(1.0f), sharkPos)
                    * targetRotation * travelRotation * initialRotation * worldRotation * initialScale * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
                GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                GLuint cc = glGetUniformLocation(shader, "customColor");
                vec3 customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trap[0][0]);
                glDrawArrays(Rendering, 0, 36);
                mat4 trapeze = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.75f, 2.75f, 4.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);
                glDrawArrays(Rendering, 0, 36);
                mat4 trapezeChild = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
                glBindVertexArray(unitCubeVAO);

                mat4 br1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -10.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.5f, 5.75f, 5.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color1;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);
                mat4 cubechild = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -10.0f, 0.0f));

                glBindVertexArray(trapezoidvao1);


                mat4 wing = cubechild * translate(mat4(1.0f), vec3(0.0f, 0.0f, 7.5f)) * rotate(glm::mat4(1.0f), glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(1.75f, 3.5f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color1;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &wing[0][0]);
                glDrawArrays(Rendering, 0, 36);

                wing = cubechild * translate(mat4(1.0f), vec3(0.0f, 0.0f, -7.5f)) * rotate(glm::mat4(1.0f), glm::radians(105.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(1.75f, 3.5f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color1;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &wing[0][0]);
                glDrawArrays(Rendering, 0, 36);

                glBindVertexArray(unitCubeVAO);

                br1 = cubechild * initialRotation2 * translate(mat4(1.0f), vec3(0.0f, -12.5f, 0.0f)) * scale(mat4(1.0f), vec3(5.5f, 5.75f, 4.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color2;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild1 = cubechild * initialRotation2 * translate(mat4(1.0f), vec3(0.0f, -12.5f, 0.0f));
                int trapezoidvao2 = createTrapezoid(0.40, 0.85, 0.1f);
                glBindVertexArray(trapezoidvao2);

                br1 = trapezeChild1 * initialRotation3 * translate(mat4(1.0f), vec3(0.0f, -11.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(5.25f, 4.75f, 3.5f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color1;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild2 = trapezeChild1 * initialRotation3 * translate(mat4(1.0f), vec3(0.0f, -12.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

                br1 = trapezeChild2 * initialRotation4 * translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(5.25f, 2.5f, 1.5f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);
            }
        }
        // Member function
        void update() {
            
            dt = glfwGetTime() - time;
            time += dt;
            sharkPos += travel * speed * dt;
            

            if (glm::abs(cAngleZ - tAngleZ) > 0.1) {
                if (cAngleZ - tAngleZ > 0) {
                   cAngleZ = cAngleZ - rotationSpeed * dt;
                  
                }
                else {
                    cAngleZ = cAngleZ + rotationSpeed * dt;
                }
                
            }

            if (glm::abs(cAngleY - tAngleY) > 0.1) {
                if (cAngleY - tAngleY > 0) {
                    cAngleY -= rotationSpeed * dt;
                }
                else {  
                    cAngleY += rotationSpeed * dt;
                }
                

            }
           // cAngleY = 0.0f;
           cAngleZ = 0.0f;
            travelRotation = rotate(mat4(1.0f), glm::radians(cAngleZ), vec3(0.0f, 0.0f, 1.0f));
            targetRotation = rotate(mat4(1.0f), glm::radians(cAngleY), vec3(0.0f, 1.0f, 0.0f));

            if (glm::distance(sharkPos, targetPos) <= 1.0f) {
                targetPos = vec3(getRandomFloat(-5.0f, 5.0f), getRandomFloat(-5.0f, 5.0f), getRandomFloat(-5.0f, 5.0f));
                travel = targetPos - sharkPos;
                travel = glm::normalize(travel);
                
                float angle = glm::acos(travel.x);
                angle = degrees(angle);
                if (travel.z < 0) {
                    angle *= -1.0f;
                }
                tAngleY = angle;
                std::cout << "T angle Y: " << tAngleY << " T angle Z: " << tAngleZ << std::endl;
                /*
                float angle = getRandomFloat(0.0f, 360.0f); //getRandomFloat(0.0f, 360.0f);
                travel = vec3(-cos(glm::radians(angle)), 0.0f, sin(glm::radians(angle)));
                targetPos = (getRandomFloat(10.0f, 30.0f) * travel + sharkPos);
                float fakeDistance = glm::distance(targetPos, sharkPos);
                targetPos.y = getRandomFloat(-30.0f,30.0f);
                //TERRAIN HEIGHT
                float angle2 = glm::acos(fakeDistance / glm::distance(sharkPos, targetPos));
                travel = targetPos - sharkPos;
                travel = glm::normalize(travel);
                
                //cAngleY = angle;
                //cAngleZ = degrees(angle2);
                tAngleY = angle;
                tAngleZ = degrees(-angle2);
                std::cout<< "T angle Y: " << tAngleY << "T angle Z: " << tAngleZ << std::endl;
                //cAngleZ = 0.0f;
                //cAngleY = tAngleY;*/
            }


        }

    private:
        float xPos = 1.0f;
        float yPos = 1.0f;
        float zPos = 1.0f;
        float time = 0.0f;
        mat4 initialRotation1 = mat4(1.0);
        mat4 initialRotation2 = mat4(1.0);
        mat4 initialRotation3 = mat4(1.0);
        mat4 initialRotation4 = mat4(1.0);
        mat4 initialRotation = mat4(1.0);
        mat4 targetRotation = mat4(1.0);
        mat4 travelRotation = mat4(1.0);
        mat4 initialPosition = mat4(1.0);
        mat4 worldRotation = mat4(1.0f);
        mat4 initialScale = mat4(1.0f);
        int trapezoidvao = createTrapezoid(0.5, 0.25, 0.25f);
        int unitCubeVAO = createUnitCube();
        float taille = 1.0f;
        vec3 sharkPos;
        vec3 travel;
        float speed = 4.0f;
        float rotationSpeed = 90.0f;
        float dt = 0.0f;
        vec3 targetPos;
        vec3 targetAngles;
        vec3 currentAngles;
        float cAngleY = 0.0f;
        float cAngleZ = 0.0f;
        float tAngleY = 0.0f;
        float tAngleZ = 0.0f;
        float coin;
        vec3 color;
        vec3 color1;
        vec3 color2;
    };

    class Fish {
    public:
        float txPos = 2.0f;
        float tyPos = 2.0f;
        float tzPos = 2.0f;
        float getRandomFloat(float min, float max) {
            return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
        }
        // Constructor
        Fish(float x) {
            // Initialize member variable
            xPos = getRandomFloat(-30.0f, 30.0f);
            yPos = getRandomFloat(-30.0f, 30.0f);
            zPos = getRandomFloat(-30.0f, 30.0f);
            txPos = xPos + 0.9f;
            tyPos = yPos + 0.9f;
            tzPos = zPos + 0.9f;
            taille = 0.005625f * x;
            fishPos = vec3(xPos, yPos, zPos);
            targetPos = vec3(txPos, tyPos, tzPos);
            travel = targetPos - fishPos;
            travel = glm::normalize(travel);
            float a = getRandomFloat(0.0f, 1.0f);
            float b = getRandomFloat(0.0f, 1.0f);
            float c = getRandomFloat(0.0f, 1.0f);
            color = vec3(a, b, c);
            color1 = vec3(b, a, b);
            color2 = vec3(c, b, a);
            coin = getRandomFloat(-1.0f, 1.0f);
        }
        vec3 getPosition() {
            return fishPos;
        }
        void draw() {
            if (coin < 0.0f) {
                update();
                time = glfwGetTime();
                initialRotation = rotate(mat4(1.0f), glm::radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
                initialScale = scale(mat4(1.0f), vec3(taille, taille, taille));
                initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(time * 2) * 20), vec3(1.0f, 0.0f, 0.0f));
                initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 20) * 20), vec3(0.0f, 1.0f, 0.0f));
                initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 40) * 20), vec3(0.0f, 1.0f, 0.0f));
                initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 60) * 20), vec3(1.0f, 0.0f, 0.0f));
               

                GLuint Rendering = GL_TRIANGLES;
                glBindVertexArray(trapezoidvao);
                mat4 trap = initialPosition * translate(mat4(1.0f), fishPos)
                    * travelRotation * targetRotation * initialRotation * worldRotation * initialScale * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
                mat4 trapeze = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
                GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                GLuint cc = glGetUniformLocation(shader, "customColor");
                vec3 customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                glBindVertexArray(unitCubeVAO);

                mat4 br1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

                br1 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild2 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f));

                glBindVertexArray(trapezoidvao);
                trapeze = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild3 = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));


                trapeze = trapezeChild3 * initialRotation4 * translate(mat4(1.0f), vec3(0.0f, 2.75f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 2.0f, 1.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

                glDrawArrays(Rendering, 0, 36);
            }
            else if(coin ==0.0f){
                update();
                time = glfwGetTime();
                initialRotation = rotate(mat4(1.0f), glm::radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
                initialScale = scale(mat4(1.0f), vec3(taille, taille, taille));
                initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(time * 2) * 20), vec3(1.0f, 0.0f, 0.0f));
                initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 20) * 20), vec3(0.0f, 1.0f, 0.0f));
                initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 40) * 20), vec3(0.0f, 1.0f, 0.0f));
                initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 60) * 20), vec3(1.0f, 0.0f, 0.0f));
               

                GLuint Rendering = GL_TRIANGLES;
                glBindVertexArray(trapezoidvao);
                mat4 trap = initialPosition * translate(mat4(1.0f), fishPos)
                    * travelRotation * targetRotation * initialRotation * worldRotation * initialScale * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
                mat4 trapeze = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
                GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                GLuint cc = glGetUniformLocation(shader, "customColor");
                vec3 customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                glBindVertexArray(unitCubeVAO);

                mat4 br1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

                br1 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild2 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f));

                glBindVertexArray(trapezoidvao);
                trapeze = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild3 = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));


                trapeze = trapezeChild3 * initialRotation4 * translate(mat4(1.0f), vec3(0.0f, 2.75f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 2.0f, 1.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = vec3(getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f), getRandomFloat(0.0f, 1.0f));
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

                glDrawArrays(Rendering, 0, 36);
            }
            else {
                update();
                time = glfwGetTime();
                initialRotation = rotate(mat4(1.0f), glm::radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
                initialScale = scale(mat4(1.0f), vec3(taille, taille, taille));
                initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(time * 2) * 20), vec3(1.0f, 0.0f, 0.0f));
                initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 20) * 20), vec3(0.0f, 1.0f, 0.0f));
                initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 40) * 20), vec3(0.0f, 1.0f, 0.0f));
                initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 60) * 20), vec3(1.0f, 0.0f, 0.0f));
                

                GLuint Rendering = GL_TRIANGLES;
                glBindVertexArray(trapezoidvao);
                mat4 trap = initialPosition * translate(mat4(1.0f), fishPos)
                    * travelRotation * targetRotation * initialRotation * worldRotation * initialScale * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
                mat4 trapeze = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
                GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                GLuint cc = glGetUniformLocation(shader, "customColor");
                vec3 customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                glBindVertexArray(unitCubeVAO);

                mat4 br1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color1;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

                br1 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color2;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild2 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f));

                glBindVertexArray(trapezoidvao);
                trapeze = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color1;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);
                glDrawArrays(Rendering, 0, 36);

                mat4 trapezeChild3 = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));


                trapeze = trapezeChild3 * initialRotation4 * translate(mat4(1.0f), vec3(0.0f, 2.75f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 2.0f, 1.0f));
                worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
                cc = glGetUniformLocation(shader, "customColor");
                customColor = color;
                glUniform3f(cc, customColor.x, customColor.y, customColor.z);
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

                glDrawArrays(Rendering, 0, 36);
            }
        }
        void drive(GLFWwindow* window) {

            time = glfwGetTime();
            initialScale = scale(mat4(1.0f), vec3(taille, taille, taille));
            initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(time * 2) * 20), vec3(1.0f, 0.0f, 0.0f));
            initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 20) * 20), vec3(0.0f, 1.0f, 0.0f));
            initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 40) * 20), vec3(0.0f, 1.0f, 0.0f));
            initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(time * 2 + 60) * 20), vec3(1.0f, 0.0f, 0.0f));
            

            GLuint Rendering = GL_TRIANGLES;
            glBindVertexArray(trapezoidvao);
            mat4 trap = initialPosition * translate(mat4(1.0f), fishPos)
                * initialRotation * worldRotation * initialScale * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
            mat4 trapeze = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
            GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            GLuint cc = glGetUniformLocation(shader, "customColor");
            vec3 customColor = vec3(1.0f, 0.647f, 0.0f);
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild = trap * translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            glBindVertexArray(unitCubeVAO);

            mat4 br1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(1.0f, 1.0f, 1.0f);
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild1 = trapezeChild * initialRotation1 * translate(mat4(1.0f), vec3(0.0f, -8.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            br1 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(2.0f, 4.75f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(1.0f, 0.647f, 0.0f);
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &br1[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild2 = trapezeChild1 * initialRotation2 * translate(mat4(1.0f), vec3(-4.5f, 0.0f, 0.0f));

            glBindVertexArray(trapezoidvao);
            trapeze = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 4.75f, 2.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(1.0f, 1.0f, 1.0f);
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);
            glDrawArrays(Rendering, 0, 36);

            mat4 trapezeChild3 = trapezeChild2 * initialRotation3 * translate(mat4(1.0f), vec3(-7.5f, 0.0f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));


            trapeze = trapezeChild3 * initialRotation4 * translate(mat4(1.0f), vec3(0.0f, 2.75f, 0.0f)) * rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(4.75f, 2.0f, 1.0f));
            worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            cc = glGetUniformLocation(shader, "customColor");
            customColor = vec3(1.0f, 0.647f, 0.0f);
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &trapeze[0][0]);

            glDrawArrays(Rendering, 0, 36);
            //Character Movement
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // Move Racket along negative x-axis
            {
                initialPosition *= translate(mat4(1.0f), vec3(0.25f, 0.0f, 0.0f) * 0.2f);
            }
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // Move Racket along negative x-axis
            {
                initialPosition *= translate(mat4(1.0f), vec3(-0.25f, 0.0f, 0.0f) * 0.2f);
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // Move Racket along positive z-axis
            {
                initialPosition *= translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.25f) * 0.2f);
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Move Racket along negative z-axis
            {
                initialPosition *= translate(mat4(1.0f), vec3(0.0f, 0.0f, -0.25f) * 0.2f);
            }

            if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) // Move Racket along negative x-axis
            {
                initialPosition *= translate(mat4(1.0f), vec3(0.0f, 0.25f, 0.0f) * 0.2f);
            }
            if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) // Move Racket along negative x-axis
            {
                initialPosition *= translate(mat4(1.0f), vec3(0.0f, -0.25f, 0.0f) * 0.2f);
            }

            //BONUS POINT? Racket Rotation
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // Rotate Racket positively about y-axis
            {
                initialRotation *= rotate(mat4(1.0f), 0.0872665f, vec3(0.0f, 0.25f, 0.0f) * 0.2f);
            }
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // Rotate Racket negatively about y-axis
            {
                initialRotation *= rotate(mat4(1.0f), -0.0872665f, vec3(0.0f, 0.25f, 0.0f) * 0.2f);
            }
        }
        // Member function
        void update() {
            //time = glfwGetTime();
            dt = glfwGetTime() - time;
            time += dt;
            fishPos += travel * speed * dt;

            if (glm::abs(cAngleZ - tAngleZ) > 0.1) {
                if (cAngleZ - tAngleZ < 0) {
                    cAngleZ = cAngleZ + rotationSpeed * dt;

                }
                else {
                    cAngleZ = cAngleZ - rotationSpeed * dt;
                }

            }



            if (glm::abs(cAngleY - tAngleY) > 0.1) {
                if (cAngleY - tAngleY < 0) {
                    cAngleY += rotationSpeed * dt;
                }
                else {
                    cAngleY -= rotationSpeed * dt;
                }


            }

            cAngleZ = 0.0f;
            travelRotation = rotate(mat4(1.0f), glm::radians(cAngleZ), vec3(0.0f, 0.0f, 1.0f));
            targetRotation = rotate(mat4(1.0f), glm::radians(cAngleY), vec3(0.0f, 1.0f, 0.0f));

            if (glm::distance(fishPos, targetPos) <= 1.0f) {
                /*float angle = getRandomFloat(-180.0f, 180.0f);
                travel = vec3(-cos(glm::radians(angle)), 0.0f, sin(glm::radians(angle)));
                targetPos = (getRandomFloat(10.0f, 30.0f) * travel + fishPos);
                float fakeDistance = glm::distance(targetPos, fishPos);
                targetPos.y = getRandomFloat(-20.0f, 20.0f);
                //TERRAIN HEIGHT
                float angle2 = glm::acos(fakeDistance / glm::distance(fishPos, targetPos));
                travel = targetPos - fishPos;
                travel = glm::normalize(travel);

                tAngleY = angle;
                tAngleZ = degrees(angle2);
                */
                targetPos = vec3(getRandomFloat(-5.0f, 5.0f), getRandomFloat(-5.0f, 5.0f), getRandomFloat(-5.0f, 5.0f));
                travel = targetPos - fishPos;
                travel = glm::normalize(travel);

                float angle = glm::acos(travel.x);
                angle = degrees(angle);
                if (travel.z < 0) {
                    angle *= -1.0f;
                }
                tAngleY = angle;
                std::cout << "T angle Y: " << tAngleY << " T angle Z: " << tAngleZ << std::endl;




            }
        }

    private:
        float xPos = 1.0f;
        float yPos = 1.0f;
        float zPos = 1.0f;
        float time = 0.0f;
        mat4 initialRotation1 = mat4(1.0);
        mat4 initialRotation2 = mat4(1.0);
        mat4 initialRotation3 = mat4(1.0);
        mat4 initialRotation4 = mat4(1.0);
        mat4 initialRotation = mat4(1.0);
        mat4 initialPosition = mat4(1.0);
        mat4 worldRotation = mat4(1.0f);
        mat4 targetRotation = mat4(1.0);
        mat4 travelRotation = mat4(1.0);
        mat4 initialScale = mat4(1.0f);
        int trapezoidvao = createTrapezoid(0.5, 0.25, 0.25f);
        int unitCubeVAO = createUnitCube();
        float taille = 1.0f;
        vec3 fishPos;
        vec3 targetPos;
        vec3 travel;
        float speed = 6.0f;
        float dt = 0.0f;
        float cAngleY = 0.0f;
        float cAngleZ = 0.0f;
        float tAngleY = 0.0f;
        float tAngleZ = 0.0f;
        float rotationSpeed = 90.0f;
        float coin;
        vec3 color;
        vec3 color1;
        vec3 color2;
    };
    std::vector<Shark> sharkList;
    std::vector<Fish> fishList;
    float getRandomFloat(float min, float max);
    int fishCount = 0;
    int sharkCount = 0;
   for (int i = 1; i <= 20; ++i) {
        float coin = getRandomFloat(-30.0f, 30.0f);
        if (coin < 0.0f) {
            Fish fish(getRandomFloat(5.0f, 15.0f));
            fishList.push_back(fish);
            fishCount++;
        }
        else {
            Shark shark(getRandomFloat(5.0f, 15.0f));
            sharkList.push_back(shark);
            sharkCount++;
        }
        
    }

  //  Fish f1 = Fish(15.0f);
    //Fish f2 = Fish(1.0f);
    //Shark s1 = Shark(8.5f);
    //Shark s2 = Shark(10.5f);
    // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Binding VAO for Line 
        glBindVertexArray(vao1);

        mat4 groundMatrix;
        // Drawing 100 Lines Along X
        for (int i = 50; i >= -50; i--)
        {
            groundMatrix = translate(mat4(1.0f), vec3(i, 0.0f, 0.0f))
                * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f))
                * worldRotation;
            GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            GLuint cc = glGetUniformLocation(shader, "customColor");
            vec3 customColor = vec3(1.0f, 1.0f, 0.0f);
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundMatrix[0][0]);

            glDrawArrays(GL_LINES, 0, 6);
        }

        // Drawing 100 lines along Z
        for (int i = 50; i >= -50; i--)
        {
            mat4 groundMatrix2 = groundMatrix * translate(mat4(1.0f), vec3(50.0f, 0.0f, i))
                * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f))
                * rotate(mat4(1.0f), 1.5708f, vec3(0.0f, 1.0f, 0.0f));
            GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
            GLuint cc = glGetUniformLocation(shader, "customColor");
            vec3 customColor = vec3(1.0f, 1.0f, 0.0f);
            glUniform3f(cc, customColor.x, customColor.y, customColor.z);
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundMatrix2[0][0]);

            glDrawArrays(GL_LINES, 0, 6);
        }

        glBindVertexArray(unitCubeVAO);

        // Z Axis red
        mat4 zAxisWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 2.5f))
            * scale(mat4(1.0f), vec3(0.05f, 0.05f, 2.5f))
            * worldRotation;
        GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
        GLuint cc = glGetUniformLocation(shader, "customColor");
        vec3 customColor = vec3(1.0f, 0.0f, 0.0f);
        glUniform3f(cc, customColor.x, customColor.y, customColor.z);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &zAxisWorldMatrix[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Y Axis green
        mat4 yAxisWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.5f, 0.0f))
            * scale(mat4(1.0f), vec3(0.05f, 2.5f, 0.05f))
            * worldRotation;
        worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
        cc = glGetUniformLocation(shader, "customColor");
        customColor = vec3(0.0f, 1.0f, 0.0f);
        glUniform3f(cc, customColor.x, customColor.y, customColor.z);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &yAxisWorldMatrix[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // X Axis blue
        mat4 xAxisWorldMatrix = translate(mat4(1.0f), vec3(2.5f, 0.0f, 0.0f))
            * scale(mat4(1.0f), vec3(2.5f, 0.05f, 0.05f))
            * worldRotation;
        worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
        cc = glGetUniformLocation(shader, "customColor");
        customColor = vec3(0.0f, 0.0f, 1.0f);
        glUniform3f(cc, customColor.x, customColor.y, customColor.z);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &xAxisWorldMatrix[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, 36);


      
        //Lower arm
        mat4 lowerArmMatrix = initialPosition * translate(mat4(1.0f), vec3(-10.0f, 10.0f, 10.0f))
            * initialRotation * worldRotation * initialScale * scale(mat4(1.0f), vec3(1.5f, 1.5f, 1.5f));

        //rotate(glm::mat4(1.0f), glm::radians(65.0f), glm::vec3(1.0f, 1.0f, 1.0f)) * 
        worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
        cc = glGetUniformLocation(shader, "customColor");
        customColor = vec3(1.0f, 0.0f, 0.0f);
        glUniform3f(cc, customColor.x, customColor.y, customColor.z);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &lowerArmMatrix[0][0]);

        glDrawArrays(Rendering, 0, 36); // 36 vertices, starting at index 0


        //FISH
       // f1.drive(window);
       // f2.draw();
        //Shark
       //s1.draw();
       //s2.draw();
        for (int i = 0; i < sharkCount; ++i) {
            
            sharkList[i].draw();
        }
        for (int i = 0; i < fishCount; ++i) {

            fishList[i].draw();
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();

        //INPUT HANDLING

        // Close Window upon pressing Esc
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        //Change Render Modes
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) // Change Racket render mode to Points
        {
            Rendering = GL_POINTS;
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) // Change Racket render mode to Lines
        {
            Rendering = GL_LINES;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) // Change Racket render mode to Triangles
        {
            Rendering = GL_TRIANGLES;
        }

        //Racket XZ Movement
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // Move Racket along negative x-axis
        {
            initialPosition *= translate(mat4(1.0f), vec3(0.25f, 0.0f, 0.0f) * 0.2f);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // Move Racket along negative x-axis
        {
            initialPosition *= translate(mat4(1.0f), vec3(-0.25f, 0.0f, 0.0f) * 0.2f);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // Move Racket along positive z-axis
        {
            initialPosition *= translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.25f) * 0.2f);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Move Racket along negative z-axis
        {
            initialPosition *= translate(mat4(1.0f), vec3(0.0f, 0.0f, -0.25f) * 0.2f);
        }

        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) // Move Racket along negative x-axis
        {
            initialPosition *= translate(mat4(1.0f), vec3(0.0f, 0.25f, 0.0f) * 0.2f);
        }
        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) // Move Racket along negative x-axis
        {
            initialPosition *= translate(mat4(1.0f), vec3(0.0f, -0.25f, 0.0f) * 0.2f);
        }

        //BONUS POINT? Racket Rotation
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // Rotate Racket positively about y-axis
        {
            initialRotation *= rotate(mat4(1.0f), 0.0872665f, vec3(0.0f, 0.25f, 0.0f) * 0.2f);
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // Rotate Racket negatively about y-axis
        {
            initialRotation *= rotate(mat4(1.0f), -0.0872665f, vec3(0.0f, 0.25f, 0.0f) * 0.2f);
        }
        initialRotation1 = rotate(mat4(1.0f), glm::radians(sin(lastFrameTime / (1.25f)) * 10), vec3(1.0f, 0.0f, 0.0f));
        initialRotation2 = rotate(mat4(1.0f), glm::radians(-sin(lastFrameTime / (1.50f)) * 9), vec3(1.0f, 0.0f, 0.0f));
        initialRotation3 = rotate(mat4(1.0f), glm::radians(-sin(lastFrameTime / (1.75f)) * 9), vec3(1.0f, 0.0f, 0.0f));
        initialRotation4 = rotate(mat4(1.0f), glm::radians(-sin(lastFrameTime / (2.0f)) * 8), vec3(1.0f, 0.0f, 0.0f));

        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) // Rotate Racket positively about y-axis
        {
            initialRotation1 *= rotate(mat4(1.0f), 0.0872665f, vec3(1.0f, 0.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) // Rotate Racket negatively about y-axis
        {
            initialRotation1 *= rotate(mat4(1.0f), -0.0872665f, vec3(1.0f, 0.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) // Rotate Racket positively about y-axis
        {
            initialRotation2 *= rotate(mat4(1.0f), 0.0872665f, vec3(0.0f, 1.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) // Rotate Racket negatively about y-axis
        {
            initialRotation2 *= rotate(mat4(1.0f), -0.0872665f, vec3(0.0f, 1.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) // Rotate Racket positively about y-axis
        {
            initialRotation3 *= rotate(mat4(1.0f), 0.0872665f, vec3(0.0f, 1.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) // Rotate Racket negatively about y-axis
        {
            initialRotation3 *= rotate(mat4(1.0f), -0.0872665f, vec3(0.0f, 1.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) // Rotate Racket positively about y-axis
        {
            initialRotation4 *= rotate(mat4(1.0f), 0.0872665f, vec3(1.0f, 0.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) // Rotate Racket negatively about y-axis
        {
            initialRotation4 *= rotate(mat4(1.0f), -0.0872665f, vec3(1.0f, 0.0f, 0.0f));
        }


        //Racket Scaling
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) // Scale Racket up
        {
            initialScale *= scale(mat4(1.0f), 1.01f * vec3(1.0f, 1.0f, 1.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) // scale Racket down
        {
            initialScale *= scale(mat4(1.0f), 0.99f * vec3(1.0f, 1.0f, 1.0f));
        }

        //Change World Orientation
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // rotate world about positive x-axis
        {
            worldOrientationMatrix *= rotate(mat4(1.0f), 0.05f, vec3(0.25f, 0.0f, 0.0f) * 0.2f);
            glUniformMatrix4fv(worldOrientationMatrixLocation, 1, GL_FALSE, &worldOrientationMatrix[0][0]);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // rotate world about negative x-axis
        {
            worldOrientationMatrix *= rotate(mat4(1.0f), 0.05f, vec3(-0.25f, 0.0f, 0.0f) * 0.2f);
            glUniformMatrix4fv(worldOrientationMatrixLocation, 1, GL_FALSE, &worldOrientationMatrix[0][0]);
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) // rotate world about positive x-axis
        {
            worldOrientationMatrix *= rotate(mat4(1.0f), 0.05f, vec3(0.0f, 0.25f, 0.0f) * 0.2f);
            glUniformMatrix4fv(worldOrientationMatrixLocation, 1, GL_FALSE, &worldOrientationMatrix[0][0]);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) // rotate world about negative x-axis
        {
            worldOrientationMatrix *= rotate(mat4(1.0f), 0.05f, vec3(0.0f, -0.25f, 0.0f) * 0.2f);
            glUniformMatrix4fv(worldOrientationMatrixLocation, 1, GL_FALSE, &worldOrientationMatrix[0][0]);
        }


        // Reset Racket, Camera and World Orientation back to what it was at the beginning
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        {
            worldOrientationMatrix = mat4(1.0f);
            glUniformMatrix4fv(worldOrientationMatrixLocation, 1, GL_FALSE, &worldOrientationMatrix[0][0]);
            initialPosition = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
            initialScale = scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
            initialRotation = rotate(mat4(1.0f), 0.0f, vec3(0.0f, 1.0f, 0.0f));
            initialRotation4 = rotate(mat4(1.0f), 0.0f, vec3(0.0f, 1.0f, 0.0f));
            initialRotation3 = rotate(mat4(1.0f), 0.0f, vec3(0.0f, 1.0f, 0.0f));
            initialRotation2 = rotate(mat4(1.0f), 0.0f, vec3(0.0f, 1.0f, 0.0f));
            initialRotation1 = rotate(mat4(1.0f), 0.0f, vec3(0.0f, 1.0f, 0.0f));
            mat4 projectionMatrix = glm::perspective(70.0f,
                1024.0f / 768.0f,
                0.01f, 200.0f);

            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

            cameraLookAt = vec3(-10.0f, -5.0f, -10.0f);
        }

        //Racket Random Grid Location Teleport
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // Put Racket in a random location on the grid
        {
            srand(time(NULL));

            RandomGridLocation = true;
            randomGridLocationX = rand() % (50 - (-50) + 1) + (-50);
            randomGridLocationZ = rand() % (50 - (-50) + 1) + (-50);
            targetPoint = vec3((float)randomGridLocationX, 0.0f, (float)randomGridLocationZ);
            directionVector = targetPoint - glm::vec3(initialPosition[3]);

        }
        if (RandomGridLocation) // Smoothly move Racket to new position on the grid
        {
            initialPosition *= translate(mat4(1.0f), 0.1f * directionVector);

            if ((glm::vec3(initialPosition[3]).x - targetPoint.x < 0.1) && (glm::vec3(initialPosition[3]).z - targetPoint.z < 0.1))
                RandomGridLocation = false;
        }

        // Camera Pan
        if (lastMouseRightState == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            double mousePosX, mousePosY;
            glfwGetCursorPos(window, &mousePosX, &mousePosY);

            double dx = mousePosX - lastMousePosX;
            double dy = 0.0f;

            lastMousePosX = mousePosX;
            lastMousePosY = mousePosY;

            // Convert to spherical coordinates
            const float cameraAngularSpeed = 60.0f;
            cameraHAngle -= dx * cameraAngularSpeed * dt;
            cameraVAngle -= dy * cameraAngularSpeed * dt;

            // Clamp vertical angle to [-85, 85] degrees
            cameraVAngle = std::max(-85.0f, std::min(85.0f, cameraVAngle));
            if (cameraHAngle > 360)
            {
                cameraHAngle -= 360;
            }
            else if (cameraHAngle < -360)
            {
                cameraHAngle += 360;
            }

            float theta = radians(cameraHAngle);
            float phi = radians(cameraVAngle);

            cameraLookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
            vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));

        }

        // Camera Tilt
        if (lastMouseMiddleState == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            double mousePosX, mousePosY;
            glfwGetCursorPos(window, &mousePosX, &mousePosY);

            //double dx = mousePosX - lastMousePosX;
            double dy = mousePosY - lastMousePosY;

            lastMousePosX = mousePosX;
            lastMousePosY = mousePosY;

            // Convert to spherical coordinates
            const float cameraAngularSpeed = 60.0f;
            //cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
            cameraVAngle -= dy * cameraAngularSpeed * dt;

            //Clamp vertical angle to [-85, 85] degrees
            cameraVAngle = std::max(-85.0f, std::min(85.0f, cameraVAngle));
            if (cameraHAngle > 360)
            {
                cameraHAngle -= 360;
            }
            else if (cameraHAngle < -360)
            {
                cameraHAngle += 360;
            }

            float theta = radians(cameraHAngle);
            float phi = radians(cameraVAngle);

            cameraLookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
            vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));

        }

        // Camera Zoom In/Out
        if (lastMouseLeftState == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double mousePosX, mousePosY;
            glfwGetCursorPos(window, &mousePosX, &mousePosY);

            //double dx = mousePosX - lastMousePosX;
            double dy = (mousePosY - lastMousePosY) / 100;

            mat4 projectionMatrix = glm::perspective(70 + (float)dy,
                1024.0f / 768.0f,
                0.01f, 200.0f);

            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

        }

        // View Matrix
        mat4 viewMatrix = mat4(1.0);
        viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

        GLuint viewMatrixLocation = glGetUniformLocation(shader, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

    }

    // Shutdown GLFW
    glfwTerminate();

    return 0;
}

GLuint loadTexture(const char* filename)
{
    // Step1 Create and bind textures
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);


    glBindTexture(GL_TEXTURE_2D, textureId);

    // Step2 Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Step3 Load Textures with dimension data
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // Step4 Upload the texture to the PU
    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
        0, format, GL_UNSIGNED_BYTE, data);

    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}
