#include "global.h"
#include "ram.h"
struct E {
    u8 p[0x78];
};
void OverlaySlot_InitializeExtended(u8 *, struct E *, s32);
void AnimData_Load(u8 *, u32);
u32 ScriptProc_Start(u32, u32);
u32 Ratatosk_InitAnimatedParticlePool(u8 *a)
{
    struct E *e;
    s32 i;
    gRatatoskParticlePool = a;
    *(u32 *)(a + 0x18) = 0;
    e = (struct E *)(a + 0x38);
    i = 0;
    do {
        OverlaySlot_InitializeExtended(a, e, i);
        i++;
        e++;
    } while (i <= 0x1f);
    AnimData_Load(a + 0x1c, 0x8E5A);
    *(u32 *)(a + 0xF38) = ScriptProc_Start(0x922E, 0x4363);
    return 0;
}
