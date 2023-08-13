#pragma once

#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <vector>
#include <algorithm>

#include "Vertex.h"
#include "YAndNormal.h"

class GenerateTerrain {
    private:
        GLuint VBO, VAO;
        int shaderProgram;

        int grid_size;
        float iso_surface_level;
        int seed;

        glm::vec3 chunk_position;

        std::vector<Vertex> vertices;

        glm::vec3 calculateNormal(glm::vec3 e1, glm::vec3 e2, glm::vec3 e3);
        bool normalPerpendicular(glm::vec3 n);

        float densityFunc(glm::vec3 point);
        float generateRandomNumber(float minValue, float maxValue);

    public:
        GenerateTerrain(int grid_size, float iso_surface_level, glm::vec3 chunk_position, int seed, int shaderProgram); // default constructor

        std::vector<YAndNormal> findMatchingYValues(float x, float y);
        void generateChunkTerrain();
        void setupBuffers();
        void drawChunk();

        std::vector<Vertex> getVertices();
        int getGridSize();
        void setGridSize(int grid_size);
        float getIsoSL();
        void setIsoSL(float surface_level);
};