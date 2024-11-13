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

    int init()
    {
        display.voxel_init();
        return 0;
    }
    int loop()
    {
        std::thread([&]()
                    { display.voxel_loop(); })
            .join();
        // display.voxel_loop();
        return 0;
    }

private:
    int value;
    vis::voxel::display_vox display;
};

std::mutex data_mutex;

void process_data(pybind11::array_t<double> input_array)
{
    pybind11::buffer_info buffer = input_array.request();
    double *ptr = static_cast<double *>(buffer.ptr);

    data_mutex.lock();
    for (ssize_t i = 0; i < buffer.shape[0]; ++i)
    {
        // 处理每个元素
        for (ssize_t j = 0; j < buffer.shape[1]; ++j)
        {
            for (ssize_t k = 0; k < buffer.shape[2]; ++k)
            {
                // 示例：访问数据
                double value = ptr[i * buffer.shape[1] * buffer.shape[2] + j * buffer.shape[2] + k];
                // 进行你所需的操作
            }
        }
    }
    data_mutex.unlock();
};

PYBIND11_MODULE(example_module, m)
{
    pybind11::class_<TeaVis>(m, "TeaVis")
        .def(pybind11::init<int>())
        .def("check_normal", &TeaVis::check_normal)
        .def("init", &TeaVis::init)
        .def("loop", &TeaVis::loop);
}
