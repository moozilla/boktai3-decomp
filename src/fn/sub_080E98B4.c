#include "global.h"
/* Ivy_UpdateActiveState_080E98B4: same as Ivy_UpdateActiveState without setting the owner's byte +7 (provisional name) */

void SceneActorAnimation_SelectDirectionalFlags(void *, u32);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void Clr(u16 *q, u32 m)
{
    *q = m & *q;
}

void Ivy_UpdateActiveState_080E98B4(u8 *p)
{
    if (TakeFlag(p + 0x2B2)) {
        u16 *q;
        q = (u16 *)(p + 0x13A);
        *q = (*q & 0xFFFB) | 8;
        Clr((u16 *)(p + 0x2C0), 0xFFFFF7FF);
    }
    SceneActorAnimation_SelectDirectionalFlags(p, 0xE);
}
