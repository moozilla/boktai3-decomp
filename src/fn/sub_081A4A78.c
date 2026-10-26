#include "global.h"

struct G {
    u8 f[0x62];
    u16 cnt;
};
struct A {
    u8 f0[8];
    u32 flags;
    u8 f0c[0x64];
    u8 f70[0x74];
    u16 he4;
};
extern struct G *gHresvelgrProjectilePool;
void ScaledShadow_Destroy(void *);
void Hresvelgr_SetFallingAttackState(void *, s32);

void Hresvelgr_ReleaseProjectile(struct A *p)
{
    gHresvelgrProjectilePool->cnt--;
    p->he4 = 0;
    p->flags |= 1;
    ScaledShadow_Destroy(p->f70);
    Hresvelgr_SetFallingAttackState(p, 0);
}
