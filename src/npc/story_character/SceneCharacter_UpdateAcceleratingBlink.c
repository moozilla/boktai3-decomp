#include "global.h"
s32 Actor_UpdateAcceleratingBlink(u8 *, u32, u32);
static inline void orr(u16 *a, u32 m)
{
    *a = m | *a;
}
void SceneCharacter_UpdateAcceleratingBlink(u8 *a, u8 *b, u32 c)
{
    if (b[9]) {
        b[9] = 0;
        b[7] = 0;
        *(u32 *)(b + 0x14) = 0x10;
    }
    if (Actor_UpdateAcceleratingBlink(b, c, 0x10)) {
        orr((u16 *)(b + 0xe), 0x100);
        b[7] = 1;
    }
}
