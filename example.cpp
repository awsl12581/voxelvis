#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h> // 添加 STL 支持
#include "src/voxelvis.h"
#include "src/common.h"
#include "src/bridge.hpp"
#include <thread>
#include <mutex>

class TeaVis
{
public:
    TeaVis(int value) : value(value) {}

    int check_normal() const
    {
        std::cout << "Hello check success! the value is:" << value << std::endl;
        return value;
    }

    /**
     * @brief
     *
     * @return int
     */
    int init()
    {
        display.voxel_init();
        return 0;
    }
    int loop()
    {
        // std::thread([&]()
        //             { display.voxel_loop(); })

        //     .join();
        display.voxel_loop();
        return 0;
    }

    int async_thread_exec()
    {
        std::thread([&]()
                    {
                        display.voxel_init();
                        display.voxel_loop(); })

            .detach();
        return 0;
    }

private:
    int value;
    vis::voxel::display_vox display;
};

// 将 glm::vec3 转换为 Python 元组
std::tuple<float, float, float> vec3_to_tuple(const glm::vec3 &vec)
{
    return std::make_tuple(vec.x, vec.y, vec.z);
}

// 将 Python 元组转换为 glm::vec3
glm::vec3 tuple_to_vec3(const std::tuple<float, float, float> &tup)
{
    return glm::vec3(std::get<0>(tup), std::get<1>(tup), std::get<2>(tup));
}

void get_and_vis_vec3(const vis::voxel::voxel_cofig &data)
{

    std::cout << data.position.x << " " << data.position.y << " " << data.position.z << std::endl;
}
int i = 1;
void set_occ_render_data(const std::vector<vis::voxel::voxel_cofig> &data)
{
    auto data_ptr = new std::vector<vis::voxel::voxel_cofig>(data);

    // const int GRID_X = 500;
    // const int GRID_Y = 500;
    // const int GRID_Z = i;
    // i++;
    // std::vector<vis::voxel::voxel_cofig> voxels;
    // std::mt19937 gen;
    // std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);

    // for (int x = 0; x < GRID_X; ++x)
    // {
    //     for (int y = 0; y < GRID_Y; ++y)
    //     {
    //         for (int z = 0; z < GRID_Z; ++z)
    //         {
    //             vis::voxel::voxel_cofig voxel;
    //             voxel.position = glm::vec3(x, y, z);
    //             voxel.color = glm::vec3(colorDist(gen), colorDist(gen), colorDist(gen));
    //             voxels.push_back(voxel);
    //         }
    //     }
    // }
    // auto data_ptr = new std::vector<vis::voxel::voxel_cofig>(voxels);

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

// 声明 std::vector<voxel_cofig> 为 OPAQUE 类型，即不希望 pybind11 进行任何操作
PYBIND11_MAKE_OPAQUE(std::vector<vis::voxel::voxel_cofig>); // 声明为 OPAQUE 类型
PYBIND11_MAKE_OPAQUE(glm::vec3);                            // 声明为 OPAQUE 类型

PYBIND11_MODULE(teavoxelui, m)
{
    pybind11::class_<TeaVis>(m, "TeaVis")
        .def(pybind11::init<int>())
        .def("check_normal", &TeaVis::check_normal)
        .def("init", &TeaVis::init)
        .def("loop", &TeaVis::loop)
        .def("async_thread_exec", &TeaVis::async_thread_exec);

    pybind11::class_<vis::voxel::voxel_cofig>(m, "VoxelConfig")
        .def(pybind11::init<>())
        // .def(pybind11::init<glm::vec3, glm::vec3>())

        // 为 position 添加 getter 和 setter，使用自定义的转换函数
        .def_property("position", [](const vis::voxel::voxel_cofig &self)
                      { return vec3_to_tuple(self.position); }, [](vis::voxel::voxel_cofig &self, const std::tuple<float, float, float> &pos)
                      { self.position = tuple_to_vec3(pos); })

        // 为 color 添加 getter 和 setter，使用自定义的转换函数
        .def_property("color", [](const vis::voxel::voxel_cofig &self)
                      { return vec3_to_tuple(self.color); }, [](vis::voxel::voxel_cofig &self, const std::tuple<float, float, float> &col)
                      { self.color = tuple_to_vec3(col); });

    // 显式绑定 std::vector<voxel_cofig> 需要手动加入std_bind
    pybind11::bind_vector<std::vector<vis::voxel::voxel_cofig>>(m, "VoxelConfigVector");

    m.def("get_and_vis_vec3", &get_and_vis_vec3);
    m.def("set_occ_render_data", &set_occ_render_data);
}
