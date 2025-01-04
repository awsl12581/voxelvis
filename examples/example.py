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


def read(path):
    with open(path, "r", encoding="utf-8") as f:
        data = f.read()
        result = yaml.load(data, Loader=yaml.FullLoader)
    return result


result = read("datasets/voxformer/semantic-kitti.yaml")
print(result["color_map"][0] / 255)


obj = TeaVis(10)

print(obj.check_normal())

data = VoxelConfig()
data.position = (1, 2, 3)
get_and_vis_vec3(data)


# 创建 VoxelConfig 实例
voxel1 = VoxelConfig()
voxel1.position = (1.0, 2.0, 3.0)
voxel1.color = (0.5, 0.5, 0.5)

voxel2 = VoxelConfig()
voxel2.position = (4.0, 5.0, 6.0)
voxel2.color = (0.3, 0.3, 0.3)

# 创建 VoxelConfigVector
vector = VoxelConfigVector()
vector.append(voxel1)
vector.append(voxel2)

# 检查数据
for v in vector:
    print("Position:", v.position, "| Color:", v.color)


# 守护线程
obj.async_thread_exec()

# 调用绑定的函数
while True:
    set_occ_render_data(vector)
    sleep(1)

# obj.init()
# obj.loop()
