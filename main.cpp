#include "voxelvis.h"
int main()
{
    vis::voxel::display_vox display;

    display.voxel_init();
    display.voxel_loop();

    return 0;
}