#include "global.h"
extern const u8 gUnk_08606494[];

void sub_080FE534(void *p)
{
    *(u32 *)((u8 *)p + 0x288) = (u32)gUnk_08606494;
}
