#if !defined(_TEA_INCLUDED_CAMERA_H_)
#define _TEA_INCLUDED_CAMERA_H_

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <cmath>
#include <filesystem>
#include <fstream>

#include <iostream>
#include <string>

namespace vis
{
    namespace helper
    {

        // Default camera values
        const GLfloat YAW = -90.0f;
        const GLfloat PITCH = 0.0f;
        const GLfloat SPEED = 0.5f;
        const GLfloat SENSITIVTY = 0.5f;
        const GLfloat ZOOM = 45.0f;

        class Camera
        {
        public:
            enum CameraMovement
            {
                FORWARD,
                BACKWARD,
                LEFT,
                RIGHT,
                UP,
                DOWN
            };

            // Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch);
            Camera();
            Camera(
                GLfloat posX,
                GLfloat posY,
                GLfloat posZ,
                GLfloat upX,
                GLfloat upY,
                GLfloat upZ,
                GLfloat yaw,
                GLfloat pitch);
            glm::mat4 GetViewMatrix() const;
            glm::vec3 CameraGetPosition() const;
            void ProcessKeyboard(CameraMovement direction, GLfloat deltaTime);
            void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch);
            void ProcessMouseScroll(GLfloat yoffset);

        public:
            void Init();
            GLfloat get_Zoom() const;
            void RenderUi();

        private:
            // Calculates the front vector from the Camera's (updated) Eular Angles
            void updateCameraVectors();

        private:
            GLFWwindow *window_;

        private:
            // Camera Attributes
            glm::vec3 Position;
            glm::vec3 GoalPos; // 摄像机看向的目标  = position+front（方向向量）
            glm::vec3 Front;
            glm::vec3 Up;
            glm::vec3 Right;
            glm::vec3 WorldUp;
            // Eular Angles
            GLfloat Yaw;
            GLfloat Pitch;
            // Camera options
            GLfloat MovementSpeed;
            GLfloat MouseSensitivity;
            GLfloat Zoom;
        };
    } // namespace helper

} // namespace vis

#endif // _TEA_INCLUDED_CAMERA_H_
