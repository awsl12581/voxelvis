
#include "cmdparser.hpp"
#include "npy.hpp"
#include "src/bridge.hpp"
#include "src/common.h"
#include "voxelvis.h"
#include <filesystem>
#include <map>
#include <string>
#include <thread>
#include <vector>
#include <yaml-cpp/yaml.h>

void set_occ_render_data()
{
    // auto data_ptr = new std::vector<vis::voxel::voxel_cofig>(data);

    const int GRID_X = 200;
    const int GRID_Y = 200;
    const int GRID_Z = 16;
    std::vector<vis::voxel::voxel_cofig> voxels;
    std::mt19937 gen;
    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);

    for (int x = 0; x < GRID_X; ++x) {
        for (int y = 0; y < GRID_Y; ++y) {
            for (int z = 0; z < GRID_Z; ++z) {
                vis::voxel::voxel_cofig voxel;
                voxel.position = glm::vec3(x, y, z);
                voxel.color = glm::vec3(colorDist(gen), colorDist(gen), colorDist(gen));
                voxels.push_back(voxel);
            }
        }
    }
    auto data_ptr = new std::vector<vis::voxel::voxel_cofig>(voxels);

    if (cas_update_data(data_ptr, nullptr)) {
        std::cout << "data ptr set!" << std::endl;
        // std::cout << bridge_data->at(100).position.x << " " << bridge_data->at(100).position.y << " " <<
        // bridge_data->at(100).position.z << std::endl;
    }
    else {
        std::cout << "data ptr not set!" << std::endl;
    }
}

/**
 * @brief 命令行解析器配置
 * --map-file-path -m 映射文件路径和名称以当前exe为基准
 * --occ-file-path -o occ文件路径和名称以当前exe为基准
 *
 *
 * @param parser
 */
void configure_parser(cli::Parser& parser)
{
    parser.set_required<std::string>("map", "map-file-path", "映射文件路径和名称,以当前exe为基准");
    parser.set_required<std::string>("occ", "occ-file-path", "occ文件路径和名称,以当前exe为基准");
    parser.set_optional<int>("t", "test", 0, "随机一个用来展示的张量,当此启动时所有其他设置不可用", false);
}

auto parserMapFile(cli::Parser& parser) -> std::map<int, std::vector<int>>
{
    auto mapFilePath = parser.get<std::string>("map");
    std::cout << "map file path: " << mapFilePath << std::endl;

    YAML::Node config = YAML::LoadFile(mapFilePath);
    // 获取learning_map_inv和color_map
    std::map<int, int> learning_map_inv = config["learning_map_inv"].as<std::map<int, int>>();
    std::map<int, std::vector<int>> color_map = config["color_map"].as<std::map<int, std::vector<int>>>();

    // 创建预测类到颜色的映射表
    std::map<int, std::vector<int>> class_to_color;
    for (const auto& pair : learning_map_inv) {
        int class_id = pair.first;
        int label_id = pair.second;
        if (color_map.find(label_id) != color_map.end()) {
            class_to_color[class_id] = color_map[label_id];
        }
    }

    // 打印映射表以验证
    for (const auto& pair : class_to_color) {
        std::cout << "Class ID: " << pair.first << " -> Color: ";
        for (const auto& color : pair.second) {
            std::cout << color << " ";
        }
        std::cout << std::endl;
    }
    return class_to_color;
}

std::vector<vis::voxel::voxel_cofig>*
    parserOccFile(std::string& occFilePath, std::map<int, std::vector<int>>& class_to_color)
{
    std::cout << "occ file path: " << occFilePath << std::endl;

    // 加载npy文件

    // const std::string path(occFilePath);
    //  numpy -> uint8 c++ -> unsigned int
    npy::npy_data d = npy::read_npy<unsigned char>(occFilePath);

    std::vector<unsigned char> data = d.data;
    std::vector<unsigned long> shape = d.shape;
    bool fortran_order = d.fortran_order;

    for (size_t i = 0; i < shape.size(); i++) {
        std::cout << "shape[" << i << "]: " << shape[i] << std::endl;
    }

    auto vector = new std::vector<vis::voxel::voxel_cofig>();
    for (size_t i = 0; i < shape[0]; i++) {
        for (size_t j = 0; j < shape[1]; j++) {
            for (size_t k = 0; k < shape[2]; k++) {
                int value = data[i * shape[1] * shape[2] + j * shape[2] + k];
                if (value > 0 && value < 255) {
                    glm::vec3 position(i, j, k);
                    glm::vec3 color(
                        class_to_color[value][0] / 255.0,
                        class_to_color[value][1] / 255.0,
                        class_to_color[value][2] / 255.0);
                    vector->emplace_back(position, color);
                }
            }
        }
    }
    return vector;
}

std::vector<std::string> parserOccFiles(cli::Parser& parser, std::map<int, std::vector<int>>& class_to_color)
{
    auto occFilePath = parser.get<std::string>("occ");
    // 返回文件夹下的列表
    std::vector<std::string> npyFilePaths;
    for (const auto& entry : std::filesystem::directory_iterator(occFilePath)) {
        if (entry.path().extension() == ".npy") {
            npyFilePaths.push_back(entry.path().string());
        }
    }
    return npyFilePaths;
}

int main(int argc, char** argv)
{
    vis::voxel::display_vox display;

    cli::Parser parser(argc, argv);
    configure_parser(parser);
    parser.run_and_exit_if_error();

    std::thread([&]() {
        display.voxel_init();
        display.voxel_loop();
    }).detach();

    // 首先判断是否存在测试参数
    if (parser.get<int>("t")) {
        set_occ_render_data();
        // 将本线程主动停止五秒
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return 0;
    }

    auto class_to_color = parserMapFile(parser);
    auto fileList = parserOccFiles(parser, class_to_color);

    for (size_t i = 0; i < fileList.size(); i++) {
        auto fileName = fileList[i];
        auto data = parserOccFile(fileName, class_to_color);
        if (cas_update_data(data, nullptr)) {
            std::cout << "data ptr set!" << std::endl;
        }
        else {
            std::cout << "data ptr not set!" << std::endl;
        }
        // 将本线程主动停止五秒
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
