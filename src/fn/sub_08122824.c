#include "global.h"
struct S {
    u8 f[0x130];
    s16 a;
    u8 g[0x3c - 2 + 0x2];
    u16 c;
};
u8 SceneActor14_UpdateAnimState(void);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void SceneScheduledEffect_UpdateEntry(u8 *s)
{
    if (SceneActor14_UpdateAnimState() == 0) {
        if (*(s16 *)(s + 0x130) == 1) {
            u8 *p = s + 0xdc;
            Collider_SetPosition(p, s + 0xd4, 0);
            Collider_Refresh(p);
        }
        (*(u16 *)(s + 0x13c))++;
    }
}
