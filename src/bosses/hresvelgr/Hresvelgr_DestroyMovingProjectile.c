#include "global.h"

void Sprite_Remove(void *);
void ScaledShadow_Destroy(void *);
void Collider_Remove(void *);
struct A {
    u8 f0[0x18];
    u8 f18[0x48];
    u8 f60[0x6c];
    u8 fcc[1];
};

void Hresvelgr_DestroyMovingProjectile(struct A *p)
{
    Sprite_Remove(p->f18);
    ScaledShadow_Destroy(p->fcc);
    Collider_Remove(p->f60);
}
