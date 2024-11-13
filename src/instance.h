#ifndef _TEA_INCLUDED_INSTANCE_H_
#define _TEA_INCLUDED_INSTANCE_H_

#include "common.h"
#include "frustum.hpp"
#include <vector>
#include <thread>
#include <mutex>

namespace vis
{
    namespace voxel
    {
        struct voxel_cofig
        {
            glm::vec3 position;
            glm::vec3 color;
        };

        //
        extern std::mutex mutex;                       // 用于线程同步
        extern std::vector<voxel_cofig> visibleVoxels; // 主线程中的可见体素

        inline void frustumCullWorker(const std::vector<voxel_cofig> &voxels, const vis::helper::Frustum &frustum, int start, int end);
        inline void performFrustumCulling(const std::vector<voxel_cofig> &voxels, const vis::helper::Frustum &frustum);
        class Voxel
        {
        public:
            Voxel();
            void Display(vis::common::TeagineData *data);
            void RenderUi();
            ~Voxel();

        public:
        private:
            std::vector<voxel_cofig> CreateVoxelGrid();

        private:
            vis::helper::Frustum frustum;
            int activeBuffer = 0;
            GLuint voxelShader;
            GLuint voxelVBO, voxelVAO, instanceVBO[2], EBO;

            GLuint mLoc, vLoc, projLoc;
            glm::mat4 pMat, vMat, mMat;
            std::vector<voxel_cofig> voxels;
            const char *vert = "#version 460 core\n"
                               "layout(location = 0) in vec3 aPos;\n"
                               "layout(location = 1) in vec3 offset;\n"
                               "layout(location = 2) in vec3 aColor;\n"

                               "out vec3 vColor;"

                               "uniform mat4 model_matrix;\n"
                               "uniform mat4 view_matrix;\n"
                               "uniform mat4 proj_matrix;\n"
                               "void main()\n"
                               "{\n"
                               "    vColor = aColor;\n"
                               "    vec4 worldPos = model_matrix * vec4(aPos + offset, 1.0);\n"
                               "    gl_Position = proj_matrix * view_matrix * model_matrix * worldPos;\n"
                               "}\n";
            const char *frag = "#version 460 core\n"
                               "in vec3 vColor;\n"
                               "out vec4 FragColor;\n"
                               "uniform float fogDensity = 0.00001;\n"
                               "uniform vec3 fogColor = vec3(0.5, 0.5, 0.5);\n"
                               "void main()\n"
                               "{\n"
                               "    float distance = gl_FragCoord.z / gl_FragCoord.w;\n"
                               "    float fogFactor = exp(-pow(fogDensity * distance, 2.0));\n"
                               "    fogFactor = clamp(fogFactor, 0.0, 1.0);\n"
                               "    vec3 finalColor = mix(fogColor, vColor, fogFactor);\n"
                               "    FragColor = vec4(finalColor, 1.0);\n"
                               "}\n";
            const char *computed =

                "#version 460\n"
                "layout(local_size_x = 256) in;"

                "struct Voxel {"
                "    vec3 position;"
                "    vec3 color;"
                "};"

                "layout(std430, binding = 0) readonly buffer Voxels {"
                "   Voxel voxels[];"
                "};"

                "layout(std430, binding = 1) buffer VisibleVoxels {"
                "   Voxel visibleVoxels[];"
                "};"

                "uniform mat4 viewProjMatrix; "
                "uniform int numVoxels;   "
                "uniform float voxelSize;  "

                "atomic_uint numVisibleVoxels; "

                "void main() {"
                "uint id = gl_GlobalInvocationID.x;"
                "if (id >= numVoxels) return;"

                "Voxel voxel = voxels[id];"
                "vec4 pos = viewProjMatrix * vec4(voxel.position, 1.0);"

                "if (abs(pos.x) <= pos.w && abs(pos.y) <= pos.w && pos.z <= pos.w && pos.z >= 0.0) {"
                "uint index = atomicAdd(numVisibleVoxels, 1); "
                " visibleVoxels[index] = voxel;"
                "}"
                "}"

                ;
        };

    } // namespace voxel

} // namespac vis

#endif