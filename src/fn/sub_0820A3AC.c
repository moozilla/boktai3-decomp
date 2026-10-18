#include "global.h"
struct E {
    u8 p[0x180];
};
extern u8 *gUnk_02000600;
extern u16 gUnk_02000558;
void FanCannonManager_InitSlot(u8 *, struct E *, s32);
u32 FanCannonManager_Init(u8 *a)
{
    struct E *e;
    s32 i;
    gUnk_02000600 = a;
    *(u32 *)(a + 0x18) = 0;
    e = (struct E *)(a + 0x1c);
    i = 0;
    do {
        FanCannonManager_InitSlot(a, e, i);
        i++;
        e++;
    } while (i <= 7);
    gUnk_02000558 = 0;
    return 0;
}
