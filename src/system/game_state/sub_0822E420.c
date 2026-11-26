#include "global.h"
extern u8 gUnk_08E60944[];
u8 *sub_0822E420(u32 index)
{
    if (index > 48)
        return 0;
    return gUnk_08E60944 + index * 32;
}
