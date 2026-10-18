#include "global.h"
void RoamingObjectB_InitializePosition(u8 *);
void RoamingObjectB_InitHomeBounds(u8 *);
void RoamingObjectB_InitGraphics(u8 *);
void RoamingObjectB_InitCollider(u8 *);
void TargetRegistry_RegisterSceneActor(u8 *, u8 *, void (*)(void));
void RoamingObjectB_OnDamage(void);
void RoamingObjectB_SetState(u8 *, u32);
u32 RoamingObjectB_Initialize(u8 *p, u16 v)
{
    *(u16 *)(p + 0x18) = v;
    RoamingObjectB_InitializePosition(p);
    RoamingObjectB_InitHomeBounds(p);
    RoamingObjectB_InitGraphics(p);
    RoamingObjectB_InitCollider(p);
    TargetRegistry_RegisterSceneActor(p + 0x124, p + 0xBC, RoamingObjectB_OnDamage);
    RoamingObjectB_SetState(p, 0);
    return 0;
}
