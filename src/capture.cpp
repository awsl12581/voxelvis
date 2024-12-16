#include "capture.h"

FILE* vis::common::Capture::startFFmpeg()
{
    char cmd[1024];
    snprintf(
        cmd,
        sizeof(cmd),
        "ffmpeg -y -f rawvideo -pixel_format rgb24 -video_size %dx%d -framerate %d "
        "-i - -c:v libx264 -preset fast -pix_fmt yuv420p %s",
        width,
        height,
        fps,
        outputFilename.c_str());
    FILE* pipe = popen(cmd, "wb");
    if (!pipe) {
        std::cerr << "Error: Could not start FFmpeg." << std::endl;
        exit(1);
    }
    return pipe;
}

vis::common::Capture::Capture(int w, int h, int frameRate, const std::string& outputFile) :
    width(w),
    height(h),
    fps(frameRate),
    outputFilename(outputFile),
    ffmpegPipe(nullptr)
{
}

void vis::common::Capture::start()
{
    if (ffmpegPipe) {
        std::cerr << "Error: Recording already started!" << std::endl;
        return;
    }
    ffmpegPipe = startFFmpeg();
    std::cout << "Recording started." << std::endl;
}

void vis::common::Capture::stop()
{
    if (!ffmpegPipe) {
        std::cerr << "Error: No active recording to stop!" << std::endl;
        return;
    }
    pclose(ffmpegPipe);
    ffmpegPipe = nullptr;
    std::cout << "Recording stopped." << std::endl;
}

void vis::common::Capture::captureFrame()
{
    if (!ffmpegPipe) {
        return;
    }

    // 分配缓冲区存储像素数据
    std::vector<unsigned char> pixels(width * height * 3);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // 倒序写入每行数据
    for (int y = height - 1; y >= 0; --y) {
        fwrite(&pixels[y * width * 3], 3, width, ffmpegPipe);
    }
}

vis::common::Capture::~Capture()
{
    if (ffmpegPipe) {
        stop();
    }
}
