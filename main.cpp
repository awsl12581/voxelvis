#include "voxelvis.h"
#include "src/bridge.hpp"

int i = 1;
void set_occ_render_data()
{
    // auto data_ptr = new std::vector<vis::voxel::voxel_cofig>(data);

    const int GRID_X = 500;
    const int GRID_Y = 500;
    const int GRID_Z = i;
    i++;
    std::vector<vis::voxel::voxel_cofig> voxels;
    std::mt19937 gen;
    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);

    for (int x = 0; x < GRID_X; ++x)
    {
        for (int y = 0; y < GRID_Y; ++y)
        {
            for (int z = 0; z < GRID_Z; ++z)
            {
                vis::voxel::voxel_cofig voxel;
                voxel.position = glm::vec3(x, y, z);
                voxel.color = glm::vec3(colorDist(gen), colorDist(gen), colorDist(gen));
                voxels.push_back(voxel);
            }
        }
    }
    auto data_ptr = new std::vector<vis::voxel::voxel_cofig>(voxels);

    if (cas_update_data(data_ptr, nullptr))
    {
        std::cout << "data ptr set!" << std::endl;
        // std::cout << bridge_data->at(100).position.x << " " << bridge_data->at(100).position.y << " " << bridge_data->at(100).position.z << std::endl;
    }
    else
    {
        std::cout << "data ptr not set!" << std::endl;
    }
}
int main()
{
    vis::voxel::display_vox display;

    std::thread([&]()
                {
            display.voxel_init();
            display.voxel_loop(); })
        .detach();

    while (true)
    {
        set_occ_render_data();
    }

    return 0;
}