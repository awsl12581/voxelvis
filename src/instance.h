#ifndef _TEA_INCLUDED_INSTANCE_H_
#define _TEA_INCLUDED_INSTANCE_H_

#include "common.h"
#include "frustum.hpp"
#include <vector>
#include <thread>
#include <mutex>
#include "bridge.hpp"

namespace vis
{
    namespace voxel
    {
        //
        extern std::mutex mutex; // 用于线程同步

        inline void frustumCullWorker(const std::vector<voxel_cofig> &voxels, std::vector<voxel_cofig> &visibleVoxels, const vis::helper::Frustum &frustum, int start, int end);
        inline void performFrustumCulling(const std::vector<voxel_cofig> &voxels, std::vector<voxel_cofig> &visibleVoxels, const vis::helper::Frustum &frustum);
        class Voxel
        {
        public:
            Voxel();
            void Display(vis::common::TeagineData *data);
            void RenderUi();
            // void GetVoxel();
            ~Voxel();

        private:
            void ComputedRender();

        private:
            std::vector<voxel_cofig> CreateVoxelGrid();

        private:
            vis::helper::Frustum frustum;
            // int activeBuffer = 0;
            GLuint voxelShader;
            GLuint voxelVBO, voxelVAO, instanceVBO, EBO;
            GLuint voxelBuffer, visibleVoxelBuffer;
            GLuint counterBuffer;
            GLuint frustumCullShader;
            GLuint viewLoc;
            // light
            GLint objectColorLoc, lightColorLoc, lightPosLoc, viewPosLoc;
            glm::vec3 light_position;

            GLuint mLoc, vLoc, projLoc;
            glm::mat4 pMat, vMat, mMat;
            std::vector<voxel_cofig> voxels;
            std::vector<voxel_cofig> visibleVoxels; // 主线程中的可见体素

            const char *vert =
                "#version 450 core\n"
                "layout(location = 0) in vec3 aPos;\n"
                "layout(location = 1) in vec3 offset;\n"
                "layout(location = 2) in vec3 aColor;\n"
                "layout(location = 3) in vec3 normal;\n"
                "layout(location = 4) in vec3 uv;\n"

                "out vec3 vColor;"
                "out vec3 Normal;"
                "out vec3 FragPos;"

                "uniform mat4 model_matrix;\n"
                "uniform mat4 view_matrix;\n"
                "uniform mat4 proj_matrix;\n"
                "void main()\n"
                "{\n"
                "    vColor = aColor;\n"
                "    vec4 worldPos = model_matrix * vec4(aPos + offset, 1.0f);\n"
                "    gl_Position = proj_matrix * view_matrix * worldPos;\n"
                "    FragPos = vec3(model_matrix * vec4(aPos + offset, 1.0f));\n"
                "    Normal = mat3(transpose(inverse(model_matrix))) * normal;\n"
                "}\n";
            const char *frag =
                "#version 450 core\n"
                "in vec3 vColor;\n"
                "in vec3 FragPos;\n"
                "in vec3 Normal;\n"
                "out vec4 FragColor;\n"
                "uniform float fogDensity = 0.00001;\n"
                "uniform vec3 fogColor = vec3(0.5, 0.5, 0.5);\n"
                "uniform vec3 lightPos; "
                "uniform vec3 viewPos;"
                "uniform vec3 lightColor;"
                "void main()\n"
                "{\n"

                "    float ambientStrength = 0.1f;"
                "    vec3 ambient = ambientStrength * lightColor;"

                "    vec3 norm = normalize(Normal);"
                "    vec3 lightDir = normalize(lightPos - FragPos);"
                "    float diff = max(dot(norm, lightDir), 0.0);"
                "    vec3 diffuse = diff * lightColor;"

                "    float specularStrength = 0.5f;"
                "    vec3 viewDir = normalize(viewPos - FragPos);"
                "    vec3 reflectDir = reflect(-lightDir, norm);  "
                "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);"
                "    vec3 specular = specularStrength * spec * lightColor;  "

                "    float distance = gl_FragCoord.z / gl_FragCoord.w;\n"
                "    float fogFactor = exp(-pow(fogDensity * distance, 2.0));\n"
                "    fogFactor = clamp(fogFactor, 0.0, 1.0);\n"
                "    vec3 finalColor = (ambient + diffuse + specular) * mix(fogColor, vColor, fogFactor);\n"
                "    FragColor = vec4(finalColor, 1.0);\n"
                "}\n";
            const char *computed =

                "#version 450 core\n"
                "layout(local_size_x = 256) in;\n"

                "struct Voxel {\n"
                "    vec3 position;\n"
                "    vec3 color;\n"
                "};\n"

                "layout(std430, binding = 0) readonly buffer Voxels {\n"
                "   Voxel voxels[];\n"
                "};\n"

                "layout(std430, binding = 1) buffer VisibleVoxels {\n"
                "   Voxel visibleVoxels[];\n"
                "};\n"

                "layout(std430, binding = 2) buffer CounterBuffer {\n"
                "uint numVisibleVoxels;\n"
                "};\n"

                "uniform mat4 viewProjMatrix; \n"
                "void main() {\n"
                "uint id = gl_GlobalInvocationID.x;\n"
                "if (id >= voxels.length()) return;\n"

                "Voxel voxel = voxels[id];\n"
                "vec4 pos = viewProjMatrix * vec4(voxel.position, 1.0);\n"

                "if (abs(pos.x) <= pos.w && abs(pos.y) <= pos.w && pos.z <= pos.w && pos.z >= 0.0) {\n"
                "uint index = atomicAdd(numVisibleVoxels, 1u); \n"
                " visibleVoxels[index] = voxel;\n"
                "}\n"
                "}\n";
        };

    } // namespace voxel

} // namespac vis

#endif