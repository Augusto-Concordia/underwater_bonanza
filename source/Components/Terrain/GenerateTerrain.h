#pragma once

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <vector>
#include <algorithm>

#include "Vertex.h"
#include "YAndNormal.h"
#include "Components/Shader.h"

class GenerateTerrain {
public:
        // Material for the shader used by this object
        Shader::Material material;
private:
        // OpenGL buffers
        GLuint vertex_array_o;
        GLuint vertex_buffer_o;
        GLuint element_buffer_o;

        int grid_size;
        float iso_surface_level;
        int seed;

        glm::vec3 chunk_position;

        std::vector<Vertex> vertices;

        static glm::vec3 CalculateNormal(glm::vec3 e1, glm::vec3 e2, glm::vec3 e3);
        static bool NormalPerpendicular(glm::vec3 n);

        [[nodiscard]] float GenerateRandomNumber(float minValue, float maxValue) const;
        float DensityFunc(glm::vec3 point);

        //glm::vec3 barycentricCoordinates(glm::vec2 point, glm::vec3 A, glm::vec3 B, glm::vec3 C);

public:
        GenerateTerrain(int grid_size, float iso_surface_level, glm::vec3 chunk_position, int seed, Shader::Material _material = Shader::Material()); // default constructor

        std::vector<YAndNormal> FindMatchingYValues(float x, float y);
        void GenerateChunkTerrain(bool _newSeed = false);
        void SetupBuffers();
        void DrawChunk(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix, float _time = 0.0f, int _renderMode = GL_TRIANGLES, const Shader::Material *_material = nullptr);

        std::vector<Vertex> GetVertices();
        [[nodiscard]] int GetGridSize() const;
        void SetGridSize(int grid_size);
        [[nodiscard]] float GetIsoSL() const;
        void SetIsoSl(float surface_level);
};