#include "voxelvis.h"
#include "instance.h"
#include "auxgrid.h"
#include <string>

int vis::voxel::display_vox::windows_init()
{
    struct
    {
        int w;
        int h;
        const char *title;
    } win = {640, 480, "opengl"};

    glfwSetErrorCallback([](int error, const char *description)
                         { fprintf(stderr, "glfw error %d: %s\n", error, description); });
    if (!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    this->m_window = glfwCreateWindow(win.w, win.h, win.title, NULL, NULL);

    if (!this->m_window)
    {
        std::cout << "can not create window" << std::endl;
        return windows_close(-1);
    }

    glfwMakeContextCurrent(this->m_window);
    std::cout << "Enable Vsync" << std::endl;
    glfwSwapInterval(1);

    std::cout << "Create an OpenGL viewport" << std::endl;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "can not init GLAD";
        return windows_close(-1);
    }

    glViewport(0, 0, win.w, win.h);

    std::cout << "Set the viewport to update when handling window size changes" << std::endl;
    glfwSetFramebufferSizeCallback(
        this->m_window,
        [](GLFWwindow *window, int width, int height)
        {
            glViewport(0, 0, width, height);
            printf("The callback gets changed in the window w=[%d],h=[%d]\n", width, height);
            gobal_data::SetWindowsSize(width, height);
        });

    std::cout << "Enable depth testing";
    glEnable(GL_DEPTH_TEST);

    return windows_close(0);
}

int vis::voxel::display_vox::windows_close(int code)
{
    if (code != 0)
    {
        // 清理
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwTerminate();
    }

    return code;
}

int vis::voxel::display_vox::windows_imgui_init()
{
    const char *glsl_version = "#version 460";

    std::cout << "set Dear ImGui context " << glsl_version << std::endl;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    std::cout << "set IMGUI and GLFW comp";
    ImGui_ImplGlfw_InitForOpenGL(this->m_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // std::cout << "设置 IMGUI 字体";
    // ImGuiIO &io = ImGui::GetIO();
    // io.Fonts->AddFontFromFileTTF(
    //     "LXGWWenKaiGB-Regular.ttf",
    //     18.0f,
    //     NULL,
    //     io.Fonts->GetGlyphRangesChineseFull());
    return windows_close(0);
}

void vis::voxel::display_vox::loop()
{
    auto auxgrid = new vis::helper::AuxGrid();
    auto voxel = new vis::voxel::Voxel();

    while (!glfwWindowShouldClose(this->m_window))
    {
        // 清理帧缓冲区
        // glClearColor(0.45f, 0.55f, 0.60f, 0.00f);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        // 渲染world数据
        glm::mat4 projection = glm::perspective(
            g_camera->get_Zoom(),
            (float)gobal_data::kWidth_ / (float)gobal_data::kHeight_,
            0.1f,
            1000.0f);
        auto teagine_data = new vis::common::TeagineData();
        teagine_data->view = g_camera->GetViewMatrix();
        teagine_data->proj = projection;
        teagine_data->camera_position = g_camera->CameraGetPosition();
        teagine_data->currentTime = glfwGetTime();
        // stage_->Play();
        auxgrid->Display(teagine_data);
        voxel->Display(teagine_data);

        ///////////////////////////////////////////////////////////////////////////
        // 渲染imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 设置ImGuizmo的操作模式
        // ImGuizmo::BeginFrame();
        // ImGuizmo::SetOrthographic(true);
        // ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
        // ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

        // 渲染自定义界面
        // App::RenderUI();

        // aux_grid->Display();
        // stage_->RenderView();
        g_camera->RenderUi();
        voxel->RenderUi();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 清理缓存
        glfwSwapBuffers(this->m_window);
        // 处理外部事件
        glfwPollEvents();
        //
        // stage_->Event();
        camera_event();
    }
}

void vis::voxel::display_vox::camera_event()
{
    // Set frame time
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    using Move = vis::helper::Camera::CameraMovement;
    // Camera controls
    if (keys[GLFW_KEY_W])
    {
        g_camera->ProcessKeyboard(Move::FORWARD, deltaTime);
        std::cout << L"摄像机移动->前" << std::endl;
    }
    if (keys[GLFW_KEY_S])
    {
        g_camera->ProcessKeyboard(Move::BACKWARD, deltaTime);
        std::cout << L"摄像机移动->后" << std::endl;
    }
    if (keys[GLFW_KEY_A])
    {
        g_camera->ProcessKeyboard(Move::LEFT, deltaTime);
        std::cout << "摄像机移动->左" << std::endl;
    }
    if (keys[GLFW_KEY_D])
    {
        g_camera->ProcessKeyboard(Move::RIGHT, deltaTime);
        std::cout << "摄像机移动->右" << std::endl;
    }
    if (keys[GLFW_KEY_LEFT_SHIFT])
    {
        g_camera->ProcessKeyboard(Move::UP, deltaTime);
        std::cout << "摄像机移动->up" << std::endl;
    }
    if (keys[GLFW_KEY_LEFT_CONTROL])
    {
        g_camera->ProcessKeyboard(Move::DOWN, deltaTime);
        std::cout << "摄像机移动->up" << std::endl;
    }
}

void vis::voxel::display_vox::voxel_init()
{
    // 1.init all func
    this->windows_init();
    // 2.init imgui
    this->windows_imgui_init();
    // 3.camera init
    g_camera = new vis::helper::Camera();
    // 4.bind event
    glfwSetMouseButtonCallback(m_window, mouse_button_callback);
    // Set the required callback functions
    glfwSetKeyCallback(m_window, key_callback);
    glfwSetCursorPosCallback(m_window, mouse_callback);
    glfwSetScrollCallback(m_window, scroll_callback);
}

void vis::voxel::display_vox::voxel_loop()
{
    this->loop();
}

void vis::voxel::key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mode);

    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
    {
        return;
    }

    // cout << key << endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
        }
    }
}

void vis::voxel::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        return;
    }

    if (!gobal_data::leftMouseButtonPressed)
    {
        return;
    }

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    g_camera->ProcessMouseMovement(xoffset, yoffset, true);

    std::cout << "鼠标按住->" << xoffset << " " << yoffset << std::endl;
}

void vis::voxel::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    ImGuiIO &io = ImGui::GetIO();

    if (io.WantCaptureMouse)
    {
        return;
    }

    g_camera->ProcessMouseScroll(yoffset);

    std::cout << "鼠标滑轮->" << yoffset << std::endl;
}

void vis::voxel::mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

    ImGuiIO &io = ImGui::GetIO();

    if (io.WantCaptureKeyboard || io.WantCaptureMouse)
    {
        gobal_data::SetLeftMouseButtonPressed(false);
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            double xpos;
            double ypos;
            gobal_data::SetLeftMouseButtonPressed(true);
            glfwGetCursorPos(window, &xpos, &ypos);
            lastX = xpos;
            lastY = ypos;
            // mouse_callback(window, xpos, ypos);
        }
        else if (action == GLFW_RELEASE)
        {
            gobal_data::SetLeftMouseButtonPressed(false);
        }
    }
}

void vis::voxel::gobal_data::SetWindowsSize(int width, int height)
{
    std::lock_guard<std::mutex> lck(mtx);
    kHeight_ = height;
    kWidth_ = width;
}

void vis::voxel::gobal_data::SetLeftMouseButtonPressed(bool value)
{
    std::lock_guard<std::mutex> lck(bmtx);
    leftMouseButtonPressed = value;
}
