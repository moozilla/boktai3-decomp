#include "global.h"
struct Header {
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 offsetA;
    u32 offsetB;
    u32 offsetC;
};

void sub_081BC2BC(u8 *p)
{
    struct Header *src = *(struct Header **)(p + 0x8e0);
    struct Header h = *src;
    h.offsetA += (u32)src;
    h.offsetB += (u32)src;
    h.offsetC += (u32)src;
}
