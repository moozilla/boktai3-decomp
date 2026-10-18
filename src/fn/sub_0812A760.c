#include "global.h"
#define LinkedEffect_UpdateColliders sub_0812A760
struct LinkedEffectCollider {
    u8 unknown[6];
    u16 flags;
    u8 tail[0x4C];
};
struct LinkedEffect {
    u8 unknown[0x1C];
    s32 position[3];
    u8 pad28[0x30];
    struct LinkedEffectCollider colliders[2];
};
s32 Collider_Refresh(void *);
void Collider_SetPosition(void *, void *, u32);
static inline void ClearFlags(struct LinkedEffectCollider *p, u32 mask)
{
    p->flags &= ~mask;
}
void sub_0812A760(struct LinkedEffect *effect)
{
    struct LinkedEffectCollider *first = &effect->colliders[0];
    s32 *position = effect->position;
    struct LinkedEffectCollider *second;
    Collider_SetPosition(first, position, 0);
    second = &effect->colliders[1];
    Collider_SetPosition(second, position, 0);
    Collider_Refresh(first);
    Collider_Refresh(second);
    {
        u16 *timer = (u16 *)((u8 *)position + 0xD4);
        if (*timer) {
            (*timer)--;
            second->flags |= 4;
        } else
            ClearFlags(second, 4);
    }
}
