#include "capture.h"

vis::common::capture::capture()
{
    if (!check_ffmpeg_installed()) {
        std::cerr << "ffmpeg is not installed or not found in PATH." << std::endl;
        return;
    }
}

bool vis::common::capture::check_ffmpeg_installed()
{
    int result = system("ffmpeg -version >nul 2>&1");
    return result == 0;
}
