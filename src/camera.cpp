#include "camera.h"

namespace vis::helper
{
    Camera::Camera()
        : Front(glm::vec3(0.0f, 0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        Up = glm::vec3(0.0f, 1.0f, 0.0f);
        this->Position = glm::vec3(500.5f, 0.0f, 1.0f);
        this->WorldUp = Up;
        this->Yaw = YAW;
        this->Pitch = PITCH;
        this->updateCameraVectors();
    }

    Camera::Camera(
        GLfloat posX,
        GLfloat posY,
        GLfloat posZ,
        GLfloat upX,
        GLfloat upY,
        GLfloat upZ,
        GLfloat yaw,
        GLfloat pitch)
        : Front(glm::vec3(-1.0f, -1.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        this->Position = glm::vec3(posX, posY, posZ);
        this->WorldUp = glm::vec3(upX, upY, upZ);
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();
    }

    glm::mat4 Camera::GetViewMatrix() const
    {
        return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
    }

    glm::vec3 Camera::CameraGetPosition() const
    {
        return this->Position;
    }

    void Camera::ProcessKeyboard(CameraMovement direction, GLfloat deltaTime)
    {
        GLfloat velocity = this->MovementSpeed * deltaTime;
        if (direction == FORWARD)
        {
            this->Position += this->Front * velocity;
        }
        if (direction == BACKWARD)
        {
            this->Position -= this->Front * velocity;
        }
        if (direction == LEFT)
        {
            this->Position -= this->Right * velocity;
        }
        if (direction == RIGHT)
        {
            this->Position += this->Right * velocity;
        }
        if (direction == UP)
        {
            this->Position += this->Up * velocity;
        }
        if (direction == DOWN)
        {
            this->Position -= this->Up * velocity;
        }
    }

    void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
    {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        this->Yaw += xoffset;
        this->Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (this->Pitch > 89.0f)
            {
                this->Pitch = 89.0f;
            }
            if (this->Pitch < -89.0f)
            {
                this->Pitch = -89.0f;
            }
        }

        // Update Front, Right and Up Vectors using the updated Eular angles
        this->updateCameraVectors();
    }

    void Camera::ProcessMouseScroll(GLfloat yoffset)
    {
        if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
        {
            this->Zoom -= yoffset * 0.05;
        }
        if (this->Zoom <= 1.0f)
        {
            this->Zoom = 1.0f;
        }
        if (this->Zoom >= 45.0f)
        {
            this->Zoom = 45.0f;
        }
    }

    void Camera::updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        this->Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        this->Right = glm::normalize(glm::cross(
            this->Front,
            this->WorldUp)); // Normalize the vectors, because their length gets closer to 0 the more
                             // you look up or down which results in slower movement.
        this->Up = glm::normalize(glm::cross(this->Right, this->Front));
    }

    void Camera::RenderUi()
    {
        // 创建一个设置窗口
        ImGui::Begin(u8"camera config");
        // 显示一些文本（也可以使用字符串格式）
        ImGui::Text(u8"position");

        // 缓冲区用于存储文本输入值
        ImGui::DragFloat3(u8"xyz", &Position.x);
        if (ImGui::DragFloat3(u8"look at", &GoalPos.x))
        {
            // GOAL = position + front
            // front = goal - position
            Front = glm::normalize(Position - GoalPos);
        }

        ImGui::End();
    }

    GLfloat Camera::get_Zoom() const
    {
        return this->Zoom;
    }
}
