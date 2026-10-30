#include "display_resource.h"

s32 ResourceHeader_Relocate(void *destination, void *source)
{
    struct DisplayResourceHeader *dst = destination;
    struct DisplayResourceHeader *src = source;
    *dst = *src;
    dst->words[3] += (u32)src;
    dst->words[4] += (u32)src;
    dst->words[5] += (u32)src;
    dst->words[6] += (u32)src;
    dst->words[7] += (u32)src;
    return 0;
}
