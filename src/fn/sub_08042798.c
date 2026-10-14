#include "display_animation.h"
#define DisplayAnimation_SelectTiles sub_08042798
void sub_08219A80(void *, u32);
s32 DisplayAnimation_SelectTiles(struct DisplayAnimation *display, u32 index)
{
    if (!display->kind)
        return -1;
    if (display->kind == 1)
        AnimData_SelectTiles(&display->resource, index);
    else
        sub_08219A80((u8 *)display + 0x2C, index);
    return 0;
}
