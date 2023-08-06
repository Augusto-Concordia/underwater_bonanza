#pragma once

#include <iostream>
#include "glad/glad.h"

struct Graphics {
    inline static void ValidateFramebufferStatus() {
        // checks if the framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "ERROR -> Framebuffer is not complete! (" << glCheckFramebufferStatus(GL_FRAMEBUFFER) << "): ";

            switch (glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
                case GL_FRAMEBUFFER_UNDEFINED:
                    std::cerr << "GL_FRAMEBUFFER_UNDEFINED" << std::endl;
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                    std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                    std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl;
                    break;
                case GL_FRAMEBUFFER_UNSUPPORTED:
                    std::cerr << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                    std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << std::endl;
                    break;
                default:
                    std::cerr << "Unknown error" << std::endl;
                    break;
            }
        }
    }
};