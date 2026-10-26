#include "global.h"
#include "ram.h"
struct E {
    u8 p[0x3ac];
};
void OrbitEffect_InitSlot(u8 *, struct E *, s32);
void AnimData_Load(u8 *, u32);
u32 SceneActors_GetBySlot(u32);
u32 SunOrb_InitManager(u8 *a)
{
    struct E *e;
    s32 i;
    gSlotController = a;
    *(u32 *)(a + 0x18) = 0;
    e = (struct E *)(a + 0x38);
    i = 0;
    do {
        OrbitEffect_InitSlot(a, e, i);
        i++;
        e++;
    } while (i <= 3);
    AnimData_Load(a + 0x1c, 0x3641);
    *(u32 *)(a + 0xEF4) = SceneActors_GetBySlot(7);
    return 0;
}
