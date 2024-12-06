#include <mutex>
#include <vector>
#include <iostream>
#include <thread>
// #include <Windows.h>

static std::vector<int> *bridge_data = nullptr;
static std::mutex lck;

inline static bool cas_update_data(std::vector<int> *data, std::vector<int> **out_data)
{
    std::lock_guard<std::mutex> lock(lck); // 使用 lock_guard 自动管理锁
    if (data == nullptr)
    {
        // 说明是提取数据
        if (bridge_data == nullptr)
        {
            return false; // 无数据可用
        }
        *out_data = bridge_data; // 输出数据
    }
    else
    {
        // 说明是输入数据
        bridge_data = data;
    }
    return true;
}

void thread_1()
{
    std::vector<int> visibleVoxels;
    while (true)
    {
        auto data = new std::vector<int>();
        bool ok = cas_update_data(nullptr, &data);
        std::cout << "ok" << ok << std::endl;
        if (ok)
        {
            visibleVoxels = *data;
            std::cout << "use pipline data" << std::endl;
        }
        else
        {
            std::cout << "use origin data" << std::endl;
        }
    }
}

int main()
{

    std::thread([]()
                { thread_1(); })
        .detach();

    while (true)
    {
        std::vector<int> data = {1, 2, 3, 4, 5, 6, 7};
        auto data_ptr = new std::vector<int>(data);

        if (cas_update_data(data_ptr, nullptr))
        {
            std::cout << "data ptr set!" << std::endl;
            // std::cout << bridge_data->at(100).position.x << " " << bridge_data->at(100).position.y << " " << bridge_data->at(100).position.z << std::endl;
        }
        else
        {
            std::cout << "data ptr not set!" << std::endl;
        }
        // Sleep(1000);
    }

    return 0;
}