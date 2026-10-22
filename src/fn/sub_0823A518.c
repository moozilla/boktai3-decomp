#include "global.h"
extern u8 *gUnk_020004B4;
struct L {
    u8 pad[0x5c];
    u8 arr[1];
};
u8 sub_0823A518(u8 *p)
{
    s32 index = *(s32 *)(p + 0x18);
    if (gUnk_020004B4 == NULL || index < 0)
        return 0;
    return ((struct L *)gUnk_020004B4)->arr[index];
}