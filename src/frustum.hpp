#if !defined(_TEA_INCLUDED_FRUSTUM_H_)
#define _TEA_INCLUDED_FRUSTUM_H_

#include <glm/glm.hpp>
#include <vector>

namespace vis
{
namespace helper
{
struct Plane
{
    glm::vec3 normal;
    float distance;

    Plane(glm::vec3 n, float d) :
        normal(n),
        distance(d)
    {
    }

    // 判断点是否在平面前面
    bool isInFront(const glm::vec3& point) const { return glm::dot(normal, point) + distance > 0; }
};

class Frustum
{
public:
    std::vector<Plane> planes;

    // 从视图和投影矩阵构建视锥体
    void update(const glm::mat4& viewProjMatrix)
    {
        planes.clear();

        // 从视图投影矩阵中提取六个平面
        planes.emplace_back(
            glm::vec3(
                viewProjMatrix[0][3] + viewProjMatrix[0][0],
                viewProjMatrix[1][3] + viewProjMatrix[1][0],
                viewProjMatrix[2][3] + viewProjMatrix[2][0]),
            viewProjMatrix[3][3] + viewProjMatrix[3][0]);

        planes.emplace_back(
            glm::vec3(
                viewProjMatrix[0][3] - viewProjMatrix[0][0],
                viewProjMatrix[1][3] - viewProjMatrix[1][0],
                viewProjMatrix[2][3] - viewProjMatrix[2][0]),
            viewProjMatrix[3][3] - viewProjMatrix[3][0]);

        planes.emplace_back(
            glm::vec3(
                viewProjMatrix[0][3] + viewProjMatrix[0][1],
                viewProjMatrix[1][3] + viewProjMatrix[1][1],
                viewProjMatrix[2][3] + viewProjMatrix[2][1]),
            viewProjMatrix[3][3] + viewProjMatrix[3][1]);

        planes.emplace_back(
            glm::vec3(
                viewProjMatrix[0][3] - viewProjMatrix[0][1],
                viewProjMatrix[1][3] - viewProjMatrix[1][1],
                viewProjMatrix[2][3] - viewProjMatrix[2][1]),
            viewProjMatrix[3][3] - viewProjMatrix[3][1]);

        planes.emplace_back(
            glm::vec3(
                viewProjMatrix[0][3] + viewProjMatrix[0][2],
                viewProjMatrix[1][3] + viewProjMatrix[1][2],
                viewProjMatrix[2][3] + viewProjMatrix[2][2]),
            viewProjMatrix[3][3] + viewProjMatrix[3][2]);

        planes.emplace_back(
            glm::vec3(
                viewProjMatrix[0][3] - viewProjMatrix[0][2],
                viewProjMatrix[1][3] - viewProjMatrix[1][2],
                viewProjMatrix[2][3] - viewProjMatrix[2][2]),
            viewProjMatrix[3][3] - viewProjMatrix[3][2]);
    }

    // 检查立方体是否在视锥内
    bool isCubeInFrustum(const glm::vec3& position, float size) const
    {
        for (const auto& plane : planes) {
            // 如果立方体的每个顶点都在平面的后面，则它不在视锥体中
            glm::vec3 halfSize(size / 2.0f);
            if (!plane.isInFront(position + halfSize) && !plane.isInFront(position - halfSize)) {
                return false;
            }
        }
        return true;
    }
};
} // namespace helper

} // namespace vis

#endif // _TEA_INCLUDED_FRUSTUM_H_
