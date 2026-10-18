#include "background_controller.h"
void BackgroundMap_ClearTextRect(u32 x, u32 y, u32 width, u32 height)
{
    u16 *map = Video_GetBackgroundMap(0);
    u32 start = x;
    u32 right = x + width;
    u32 bottom = y + height;
    for (; y < bottom; y++) {
        for (x = start; x < right; x++)
            map[y * 32 + x] = 0xF001;
    }
}
