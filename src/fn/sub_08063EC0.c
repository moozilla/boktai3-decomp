#include "global.h"
void RoamingObject_InitializePosition(u8 *);
void RoamingObject_InitHomeBounds(u8 *);
void RoamingObject_InitGraphics(u8 *);
void RoamingObject_InitCollider(u8 *);
void TargetRegistry_RegisterActor(u8 *, u8 *, void (*)(void));
void RoamingObject_OnDamage(void);
void RoamingObject_SetState(u8 *, u32);
u32 RoamingObject_Initialize(u8 *p, u16 v)
{
    *(u16 *)(p + 0x18) = v;
    RoamingObject_InitializePosition(p);
    RoamingObject_InitHomeBounds(p);
    RoamingObject_InitGraphics(p);
    RoamingObject_InitCollider(p);
    TargetRegistry_RegisterActor(p + 0x124, p + 0x1c, RoamingObject_OnDamage);
    RoamingObject_SetState(p, 0);
    return 0;
}
