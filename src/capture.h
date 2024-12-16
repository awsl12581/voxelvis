#if !defined(_TEA_INCLUDED_CAPTURE_H_)
#define _TEA_INCLUDED_CAPTURE_H_

#include <cstdlib>
#include <iostream>

namespace vis
{
namespace common
{
class capture
{
public:
    capture();
    ~capture();
    void capture_screen(const char* filename);
    bool check_ffmpeg_installed();
};
} // namespace common
} // namespace vis

#endif // _TEA_INCLUDED_CAPTURE_H_
