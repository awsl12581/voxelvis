#ifndef _TEA_INCLUDED_VOXELVIS_H_
#define _TEA_INCLUDED_VOXELVIS_H_

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <mutex>
#include <string>

#include "camera.h"
#include "ui/im_image.h"

namespace vis
{

namespace voxel
{
namespace gobal_data
{
static std::mutex mtx;
static int kHeight_ = 480;
static int kWidth_ = 640;
// 全局变量，用于跟踪左键是否按下
static std::mutex bmtx;
static bool leftMouseButtonPressed = false;

void SetWindowsSize(int width, int height);

void SetLeftMouseButtonPressed(bool value);
}; // namespace gobal_data

// 用于互斥访问
static std::mutex mtx;
// 用于按键设置
static bool keys[1024];
// 全局摄像机位
static vis::helper::Camera* g_camera;
// 记录屏幕
static GLfloat lastX = 400, lastY = 300;
// 特殊标记
static bool firstMouse = true;
// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

//
class display_vox
{
private:
    /**
     * @brief 1#初始化windows窗口
     *
     * @return int
     */
    int windows_init();
    /**
     * @brief 出现错误时进行销毁
     *
     * @return int
     */
    int windows_close(int code);

    /**
     * @brief 2#初始化imgui窗口
     *
     * @return int
     */
    int windows_imgui_init();

    /**
     * @brief 事件循环
     *
     */
    void loop();

    void camera_event();

public:
    /**
     * @brief 初试化一切，初始化失败直接终止程序报错
     *
     */
    void voxel_init();

    void voxel_loop();

private:
    GLFWwindow* m_window;
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;
};

} // namespace voxel

}; // namespace vis

#endif
