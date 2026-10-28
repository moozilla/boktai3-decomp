#include "global.h"
#include "ram.h"
u8 *Video_GetBackgroundMap(u32 n)
{
    return gBgMapWorkBuffer + (n << 11);
}
