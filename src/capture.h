#if !defined(_TEA_INCLUDED_CAPTURE_H_)
#define _TEA_INCLUDED_CAPTURE_H_

#include <GLFW/glfw3.h>
#include <cstdio> // For FILE*
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

namespace vis
{
namespace common
{
class Capture
{
private:
    int width;
    int height;
    int fps;
    std::string outputFilename;
    FILE* ffmpegPipe;

    // 启动 FFmpeg 管道
    FILE* startFFmpeg();

public:
    // 构造函数
    Capture(int w, int h, int frameRate, const std::string& outputFile);

    // 开始录制
    void start();

    // 停止录制
    void stop();

    // 捕获当前帧
    void captureFrame();

    // 析构函数：清理资源
    ~Capture();
};
} // namespace common
} // namespace vis

#endif // _TEA_INCLUDED_CAPTURE_H_
