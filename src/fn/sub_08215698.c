#include "global.h"
extern u8 gUnk_02037000[];
u8 *Video_GetBackgroundMap(u32 n)
{
    return gUnk_02037000 + (n << 11);
}
