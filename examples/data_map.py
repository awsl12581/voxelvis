import numpy as np
from typing import Tuple
import sys
from time import sleep

sys.path.append("./build")

from teavoxelui import (
    TeaVis,
    VoxelConfig,
    VoxelConfigVector,
    get_and_vis_vec3,
    set_occ_render_data,
)

# coding: utf-8

import yaml

obj = TeaVis(10)

print(obj.check_normal())


def read(path):
    with open(path, "r", encoding="utf-8") as f:
        data = f.read()
        result = yaml.load(data, Loader=yaml.FullLoader)
    return result


result = read("datasets/voxformer/semantic-kitti.yaml")
learning_map_inv = result["learning_map_inv"]
color_map = result["color_map"]
# 创建预测类到颜色的映射表
class_to_color = {
    class_id: color_map[label_id]
    for class_id, label_id in learning_map_inv.items()
    if label_id in color_map
}

# 打印结果
print("Class ID to Color Map:")
for class_id, color in class_to_color.items():
    print(f"Class ID: {class_id}, Color: {color}")

array = np.squeeze(np.load("datasets/voxformer/vis/results_y_true_100.npy"))
print(array.shape)


unique_values, counts = np.unique(array, return_counts=True)
# 打印值分布
distribution = dict(zip(unique_values, counts))
print("Value Distribution:")
for value, count in distribution.items():
    print(f"Value: {value}, Count: {count}")

# 提取下标和值，并过滤掉值为 0 的数据
vector = VoxelConfigVector()

for index in np.ndindex(array.shape):
    value = array[index]
    if value > 0 and value < 255:  # 过滤掉值为 0 的数据
        data_point = VoxelConfig()  # 创建类的实例
        data_point.position = index  # 动态赋值属性
        data_point.color = (
            class_to_color[value][0] / 255.0,
            class_to_color[value][1] / 255.0,
            class_to_color[value][2] / 255.0,
        )
        vector.append(data_point)

# 打印部分结果示例
# print(f"Filtered data count: {len(vector)}")
# print(f"Sample data: {vector[:5]}")


# 守护线程
obj.async_thread_exec()

# 调用绑定的函数
while True:

    set_occ_render_data(vector)
    sleep(1)
