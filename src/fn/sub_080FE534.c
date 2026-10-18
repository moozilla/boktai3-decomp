#include "global.h"
extern const u8 gUnk_08606494[];

void Axe_InitEventCallbacks(void *p)
{
    *(u32 *)((u8 *)p + 0x288) = (u32)gUnk_08606494;
}
