#define GLEW_STATIC

#include <iostream>
#include <functional>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Components/Renderer.h"
#include "Utility/Input.hpp"

#include "SFML/Audio.hpp"

int main() {
    std::cout << "Starting..." << std::endl;

    const uint16_t INITIAL_WIDTH = 1024;
    const uint16_t INITIAL_HEIGHT = 768;

    //initialize GL context
    if (!glfwInit()) {
        fprintf(stderr, "ERROR -> Could not start GLFW\n");
        return 1;
    }

    //establish window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //initialize OS window with GLFW
    GLFWwindow* window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "Underwater Bonanza", nullptr, nullptr);
    if (window == nullptr) {
        fprintf(stderr, "ERROR -> Could not open widow through GLFW\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    //initialize GLAD
    gladLoadGL();

    //get current renderer and supported OpenGL version
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    printf("INFO -> Renderer: %s\n", renderer);
    printf("INFO -> Supported OpenGL version: %s\n", version);

    //enable depth testing and the desired testing function (the closest fragment is drawn)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //enable blending and the desired blending function
    //from: https://learnopengl.com/Advanced-OpenGL/Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetKeyCallback(window, Input::KeyCallback);

    Renderer main_renderer = Renderer(INITIAL_WIDTH, INITIAL_HEIGHT);

    int display_w, display_h, previous_display_w, previous_display_h;
    double previous_time = glfwGetTime();

    main_renderer.Init();

    while (!glfwWindowShouldClose(window)) {
        //get current display window size & update rendering
        glfwGetFramebufferSize(window, &display_w, &display_h);
        if (previous_display_w != display_w || previous_display_h != display_h)
        {
            previous_display_w = display_w;
            previous_display_h = display_h;

            main_renderer.ResizeCallback(window, display_w, display_h);
        }

        main_renderer.Render(window, glfwGetTime() - previous_time);

        //collect and process application specific input, before doing the actual polling
        Input::PreEventsPoll(window);

        //watch for any events
        glfwPollEvents();

        //collect and process application specific input, after doing the actual polling
        Input::PostEventsPoll(window);

        //stores current time for next frame
        previous_time = glfwGetTime();

        //swap buffers and prepare for next frame
        glfwSwapBuffers(window);
    }

    std::cout << "Closing..." << std::endl;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}