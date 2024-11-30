#include "instance.h"
#include <random>

namespace vis::voxel
{
    std::mutex mutex; // 用于线程同步
    void frustumCullWorker(const std::vector<voxel_cofig> &voxels, std::vector<voxel_cofig> &visibleVoxels, const vis::helper::Frustum &frustum, int start, int end)
    {

        std::vector<voxel_cofig> localVisibleVoxels;
        for (int i = start; i < end; ++i)
        {
            if (frustum.isCubeInFrustum(voxels[i].position, 1.0f))
            {
                localVisibleVoxels.push_back(voxels[i]);
            }
        }
        std::lock_guard<std::mutex> lock(mutex);
        visibleVoxels.insert(visibleVoxels.end(), localVisibleVoxels.begin(), localVisibleVoxels.end());
    }

    void performFrustumCulling(const std::vector<voxel_cofig> &voxels, std::vector<voxel_cofig> &visibleVoxels, const vis::helper::Frustum &frustum)
    {
        visibleVoxels.clear();
        int numThreads = std::thread::hardware_concurrency();
        int chunkSize = voxels.size() / numThreads;
        std::vector<std::thread> threads;

        for (int i = 0; i < numThreads; ++i)
        {
            int start = i * chunkSize;
            int end = (i == numThreads - 1) ? voxels.size() : start + chunkSize;
            threads.emplace_back(frustumCullWorker, std::ref(voxels), std::ref(visibleVoxels), std::ref(frustum), start, end);
        }

        for (auto &thread : threads)
        {
            thread.join();
        }
    }

}

vis::voxel::Voxel::Voxel()
{
    float cubeVertices[] = {
        // 顶点位置     // 顶点法线
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f};

    unsigned int cubeIndices[] = {
        // 背面
        0, 3, 2, 2, 1, 0,
        // 正面
        4, 5, 6, 6, 7, 4,
        // 左侧面
        0, 4, 7, 7, 3, 0,
        // 右侧面
        1, 2, 6, 6, 5, 1,
        // 底面
        0, 1, 5, 5, 4, 0,
        // 顶面
        3, 7, 6, 6, 2, 3};
    // 编译shader
    voxelShader = vis::common::CreateShaderProgram(vert, frag);
    glUseProgram(voxelShader);
    // 获取 MV 矩阵和投影矩阵的统一变量
    mLoc = glGetUniformLocation(voxelShader, "model_matrix");
    vLoc = glGetUniformLocation(voxelShader, "view_matrix");
    projLoc = glGetUniformLocation(voxelShader, "proj_matrix");

    glGenVertexArrays(1, &voxelVAO);
    glGenBuffers(1, &voxelVBO);
    glGenBuffers(1, &instanceVBO);
    glGenBuffers(1, &voxelBuffer);
    glGenBuffers(1, &visibleVoxelBuffer);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &counterBuffer);
    voxels = CreateVoxelGrid();

    //
    // 设置顶点缓冲区数据
    glBindVertexArray(voxelVAO);
    glBindBuffer(GL_ARRAY_BUFFER, voxelVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    // 顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, voxels.size() * sizeof(voxel_cofig), voxels.data(), GL_DYNAMIC_DRAW);

    // 设置实例属性（位置和颜色）
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(voxel_cofig), (void *)offsetof(voxel_cofig, position));
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(voxel_cofig), (void *)offsetof(voxel_cofig, color));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // 计算着色器
    // 体素数据缓冲区
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, voxels.size() * sizeof(voxel_cofig), voxels.data(), GL_STATIC_DRAW);

    // 可见体素缓冲区，容量等于所有体素（因为最差情况下所有体素都可见）
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleVoxelBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, voxels.size() * sizeof(voxel_cofig), nullptr, GL_DYNAMIC_DRAW);

    // 初始化原子计数器缓冲区
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

    frustumCullShader = vis::common::CompileComputeShader(computed);
    glUseProgram(frustumCullShader);

    //
    viewLoc = glGetUniformLocation(frustumCullShader, "viewProjMatrix");

    // 将缓冲区绑定到计算着色器
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, voxelBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleVoxelBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, counterBuffer);

    glBindVertexArray(0);
    glUseProgram(0);
}

void vis::voxel::Voxel::Display(vis::common::TeagineData *teagine_data)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    // FIXME 这里显然计算后渲染有问题，先暂且放弃
    // ComputedRender();

    glBindVertexArray(voxelVAO);

    //

    visibleVoxels = voxels;

    // 计算view proj model
    vMat = teagine_data->view;
    pMat = teagine_data->proj;
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

    //
    glUseProgram(voxelShader);

    // 传统数据绑定
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW); // 先清空缓冲区
    glBufferData(GL_ARRAY_BUFFER, visibleVoxels.size() * sizeof(voxel_cofig), visibleVoxels.data(), GL_DYNAMIC_DRAW);

    glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));
    glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

    // 实例化绘制
    glBindVertexArray(voxelVAO);
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, visibleVoxels.size());
    glBindVertexArray(0);

    glDisable(GL_DEPTH_TEST);
}

void vis::voxel::Voxel::RenderUi()
{

    ImGui::Begin("Voxel Settings");
    // if (ImGui::Checkbox("gpu", false))
    // {
    //     std::cout << "frustum GPU!" << std::endl;
    // }

    ImGui::End();
}

vis::voxel::Voxel::~Voxel() = default;

void vis::voxel::Voxel::ComputedRender()
{

    // 更新视锥体
    glm::mat4 viewProjMatrix = pMat * vMat;
    frustum.update(viewProjMatrix);
    // 剔除视锥外的体素
    // performFrustumCulling(voxels, visibleVoxels, frustum);

    // 启动计算着色器
    glUseProgram(frustumCullShader);

    // 更新Uniform变量
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewProjMatrix));

    // 绑定缓冲区并重置原子计数器
    GLuint zero = 0;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &zero);

    // 启动计算工作组，进行视锥剔除
    glDispatchCompute((voxels.size() + 127) / 128, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // 从 counterBuffer 获取可见体素数量
    GLuint visibleVoxelCount;
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &visibleVoxelCount);
    // 读取可见体素数据到CPU端的可见体素向量
    visibleVoxels.resize(visibleVoxelCount);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleVoxelBuffer);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, visibleVoxelCount * sizeof(voxel_cofig), visibleVoxels.data());
    visibleVoxels.shrink_to_fit();
    std::cout << visibleVoxelCount << "  " << visibleVoxels.size() << std::endl;
    std::cout << visibleVoxels[0].position.x << visibleVoxels[0].position.y << visibleVoxels[0].position.z << std::endl;
    std::cout << visibleVoxels[1].position.x << visibleVoxels[1].position.y << visibleVoxels[1].position.z << std::endl;
}

std::vector<vis::voxel::voxel_cofig> vis::voxel::Voxel::CreateVoxelGrid()
{
    const int GRID_X = 512;
    const int GRID_Y = 512;
    const int GRID_Z = 32;
    std::vector<voxel_cofig> voxels;
    std::mt19937 gen;
    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);

    for (int x = 0; x < GRID_X; ++x)
    {
        for (int y = 0; y < GRID_Y; ++y)
        {
            for (int z = 0; z < GRID_Z; ++z)
            {
                voxel_cofig voxel;
                voxel.position = glm::vec3(x, y, z);
                voxel.color = glm::vec3(colorDist(gen), colorDist(gen), colorDist(gen));
                voxels.push_back(voxel);
            }
        }
    }
    return voxels;
}
