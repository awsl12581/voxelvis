#if !defined(_TEA_INCLUDED_BRIDGE_H_)
#define _TEA_INCLUDED_BRIDGE_H_

#include "common.h"
#include <vector>
#include <random>
#include <mutex>

static inline std::vector<vis::voxel::voxel_cofig> *bridge_data = nullptr;
static inline std::atomic<int> data_version(0); // 数据版本
static inline std::mutex lck;
// CAS 操作，进行原子比较和交换
/**
 * @brief CAS操作
 *
 * @param data 当需要输入数据时 data输入数据，putdata为空
 * @param out_data 当需要提取数据时data为null，outdata为指针
 * @return true
 * @return false
 */
inline static bool cas_update_data(std::vector<vis::voxel::voxel_cofig> *data, std::vector<vis::voxel::voxel_cofig> **out_data)
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

#endif // _TEA_INCLUDED_BRIDGE_H_
