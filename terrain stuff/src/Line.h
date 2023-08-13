#pragma once

#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/type_ptr.hpp>

class Line {
    public:
        GLuint VBO, VAO;
        int shaderProgram;

        glm::vec3 vertices[2]; // holds vertices data for line

        glm::vec3 p1; // first vertex of line
        glm::vec3 p2; // second vertex of line
        glm::vec3 colour; // line colour

        Line(glm::vec3 p1, glm::vec3 p2, glm::vec3 colour, int shaderProgram); // default constructor
        int drawLine(); // display line , used for drawing
};