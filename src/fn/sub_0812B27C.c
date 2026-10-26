#include "global.h"
#include "ram.h"
void Sprite_Remove(u8 *);
void Collider_Remove(u8 *);
struct Slot {
    u8 sprite[0xac];
    u8 collider[0x18c - 0xac];
};
struct Pool {
    u8 pad0[0x1c];
    u32 active;
    struct Slot slots[8];
};
static inline u8 IsActive(struct Pool *p, s32 i)
{
    if (p->active & (1 << i))
        return TRUE;
    return FALSE;
}
u32 FragmentProjectilePool_Cleanup(struct Pool *p)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (IsActive(p, i)) {
            Sprite_Remove(p->slots[i].sprite);
            Collider_Remove(p->slots[i].collider);
        }
    }
    gActorEffectManager = 0;
    return 0;
}
