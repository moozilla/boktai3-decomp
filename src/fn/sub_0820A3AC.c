#include "global.h"
#include "ram.h"
struct E {
    u8 p[0x180];
};
void FanCannonManager_InitSlot(u8 *, struct E *, s32);
u32 FanCannonManager_Init(u8 *a)
{
    struct E *e;
    s32 i;
    gFanCannonManager = a;
    *(u32 *)(a + 0x18) = 0;
    e = (struct E *)(a + 0x1c);
    i = 0;
    do {
        FanCannonManager_InitSlot(a, e, i);
        i++;
        e++;
    } while (i <= 7);
    gForceDirectionMode = 0;
    return 0;
}
