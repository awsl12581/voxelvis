#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "src/voxelvis.h"
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

private:
    int value;
    vis::voxel::display_vox display;
};

PYBIND11_MODULE(teavoxelui, m)
{
    pybind11::class_<TeaVis>(m, "TeaVis")
        .def(pybind11::init<int>())
        .def("check_normal", &TeaVis::check_normal)
        .def("init", &TeaVis::init)
        .def("loop", &TeaVis::loop);
}
