import sys

sys.path.append("./build")

from teavoxelui import (
    TeaVis,
    VoxelConfig,
    VoxelConfigVector,
    get_and_vis_vec3,
    set_occ_render_data,
)

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

# 调用绑定的函数
set_occ_render_data(vector)

# obj.init()
# obj.loop()
