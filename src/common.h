#ifndef _TEA_INCLUDED_COMMON_H_
#define _TEA_INCLUDED_COMMON_H_

#include <mutex>
#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace vis
{

    namespace voxel
    {
        // position+color
        struct voxel_cofig
        {
            glm::vec3 position;
            glm::vec3 color;

            voxel_cofig() = default;
            inline voxel_cofig(const glm::vec3 &position, const glm::vec3 &color)
            {
                this->position = position;
                this->color = color;
            }
        };
    }

    namespace common
    {
        struct TeagineData
        {
            glm::mat4 view;
            glm::mat4 proj;
            glm::vec3 camera_position;
            double currentTime;
        };

        void PrintShaderLog(GLuint shader);

        void PrintProgramLog(int prog);
        bool CheckOpenGLError();

        GLuint CreateShaderProgram(const char *vert, const char *frag);
        GLuint CompileComputeShader(const char *shaderSource);

    }
}

#endif // _TEA_INCLUDED_COMMON_H_