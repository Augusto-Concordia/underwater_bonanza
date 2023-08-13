#pragma once
// https://learnopengl.com/Getting-started/Camera

#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

class Camera {

    private:
        int shaderProgram;
        int width;
        int height;

        glm::vec3 cameraPos;
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        float distance_from;
        float fov;
        float near_plane;
        float far_plane;

        double lastXPos;
        double lastYPos;

        void panCamera(float deltaX);
        void tiltCamera(float deltaY);

    public:
        Camera(int shaderProgram, int width, int height, glm::vec3 cameraPos, float fov, float near_plane, float far_plane);
        void updateCameraMatrix();
        void processCameraInputs(GLFWwindow *window);
};