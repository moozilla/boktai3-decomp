#include "global.h"
void Anim_SetSequence(void *, u32, u32, u32, u32);
void DragonPuppy_UpdateAttackTiming(void);
u32 DragonPuppy_UpdateAttackRecovery(u8 *p)
{
    u16 n = *(u16 *)(p + 0x12c);
    u32 z = 0;
    if (*(s16 *)(p + 0x12c) > 0) {
        *(u16 *)(p + 0x12c) = n - 1;
    } else {
        Anim_SetSequence(p + 0x10c, *(u32 *)(p + 0x170), 3, p[0x132], z);
        *(void (**)(void))(p + 0x108) = DragonPuppy_UpdateAttackTiming;
    }
}
