#include "global.h"

struct A {
    u8 f[0x18];
    u8 b;
    u8 g[0x4a3];
    u32 w;
};
u32 SceneActors_GetBySlot(s32);
s32 Hresvelgr_SetLinkedActor(u32, void *);

s32 GrowthHazard_InitManager(struct A *p)
{
    p->b = 0;
    p->w = SceneActors_GetBySlot(1);
    return Hresvelgr_SetLinkedActor(p->w, p);
}
