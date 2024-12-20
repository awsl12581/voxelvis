import numpy as np
from typing import Tuple
import sys
from time import sleep

sys.path.append("./build")

from teavoxelui import (
    TeaVis,
    Vec3,
    VoxelConfig,
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


result = read("datasets/occfusion/nuscenes.yaml")
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


# 打印部分结果示例
# print(f"Filtered data count: {len(vector)}")
# print(f"Sample data: {vector[:5]}")


# 守护线程
obj.async_thread_exec()

i = 1
batch_size = [1, 2, 3, 4, 5, 6, 7, 8]
# 调用绑定的函数
while i < 11:

    i = i + 1

    for batch_index in batch_size:
        batch_array = np.load(
            "datasets/occfusion/vis/occfusion_y_pred_{}.npy".format(i)
        )

        split_array = batch_array[(batch_index - 1), :, :, :]
        print(split_array.shape)
        array = np.squeeze(split_array)

        unique_values, counts = np.unique(array, return_counts=True)
        # 打印值分布
        distribution = dict(zip(unique_values, counts))
        print("Value Distribution:")
        for value, count in distribution.items():
            print(f"Value: {value}, Count: {count}")

        # 提取下标和值，并过滤掉值为 0 的数据
        # FIXME 注意这里需要多线程处理
        vector = [
            VoxelConfig(
                Vec3(*index),
                Vec3(
                    class_to_color[array[index]][0] / 255.0,
                    class_to_color[array[index]][1] / 255.0,
                    class_to_color[array[index]][2] / 255.0,
                ),
            )
            for index in np.ndindex(array.shape)
            if array[index] > 0 and array[index] < 255
        ]

        set_occ_render_data(vector)
