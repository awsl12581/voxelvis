#include "im_image.h"

void vis::im::Image::render(const std::vector<std::string>& data) const
{
    ImGui::Begin(u8"camera config");
    // 显示一些文本（也可以使用字符串格式）
    ImGui::Text(u8"position");

    // 缓冲区用于存储文本输入值
    // ImGui::DragFloat3(u8"xyz", &Position.x);
    // 键盘移动基准速度
    // ImGui::DragFloat(u8"MovementSpeed", &MovementSpeed);
    // if (ImGui::DragFloat3(u8"look at", &GoalPos.x))
    // {
    //     // GOAL = position + front
    //     // front = goal - position
    //     Front = glm::normalize(Position - GoalPos);
    // }

    ImGui::End();
}
