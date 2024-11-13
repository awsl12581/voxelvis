#include "voxelvis.h"
int main()
{
    vis::voxel::display_vox display;
    
    //std::thread([&]()
                //{/* */
            display.voxel_init();
            display.voxel_loop(); 
        //.join();

    return 0;
}