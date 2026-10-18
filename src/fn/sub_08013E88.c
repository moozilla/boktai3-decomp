#include "global.h"
void *ShadowRegistry_GetTask(void);
u8 *ShadowRegistry_FindEntry(void *, u32);
void ActorShadow_Enable(u8 *);
void ActorShadow_Disable(u8 *);
s32 ShadowRegistry_SetEntryMode(u32 a, u32 m)
{
    void *p = ShadowRegistry_GetTask();
    u8 *q;
    if (p == 0 || (q = ShadowRegistry_FindEntry(p, a)) == 0)
        return -1;
    q[2] = m;
    if ((u8)m == 1)
        ActorShadow_Enable(q + 0xc);
    else if ((u8)m == 2)
        ActorShadow_Disable(q + 0xc);
    return 0;
}
