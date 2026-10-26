#include "global.h"
#include "ram.h"
struct ProjectilePosition {
    s16 x, y, z, pad;
};
struct HresvelgrProjectile {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
    struct ProjectilePosition position;
    u8 unknown28[0x38];
    struct ProjectilePosition origin, velocity;
    u8 shadow[0x6c];
    u32 state;
    u16 stateFrame, timer, parameter, noShadow;
};
struct HresvelgrProjectilePool {
    u8 unknown0[0x18];
    u8 animation[0x4a];
    u16 count;
};
struct HresvelgrProjectile *Hresvelgr_FindFreeProjectileSlot(void);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void ScaledShadow_SetGraphic(void *, s32, s32);
u32 Hresvelgr_SetFallingAttackState(struct HresvelgrProjectile *, void *);
void sub_081A4BF8(void);
static inline void clear(void *dest)
{
    u32 zero = 0;
    CpuSet(&zero, dest, 0x05000002);
}
u32 Hresvelgr_InitializeProjectile(struct ProjectilePosition *position, u32 noShadow, u32 parameter)
{
    struct HresvelgrProjectilePool *pool = gHresvelgrProjectilePool;
    struct HresvelgrProjectile *p;
    struct ProjectilePosition *current;
    if (!position)
        return 0;
    p = Hresvelgr_FindFreeProjectileSlot();
    if (!p)
        return 0;
    ++pool->count;
    p->origin = *position;
    clear(&p->velocity);
    p->noShadow = noShadow;
    p->parameter = parameter;
    current = &p->position;
    *current = *position;
    Anim_SetKeyframes(p, pool->animation, 0x21, 2, 0);
    if (p->noShadow)
        p->flags |= 1;
    else {
        void *shadow = p->shadow;
        ScaledShadow_Initialize(shadow, current, 32, 32, 32, 32, 0x2000, 1, 8, 2, 0);
        ScaledShadow_SetGraphic(shadow, 0x1c1b, 7);
        p->flags &= ~1;
    }
    return Hresvelgr_SetFallingAttackState(p, sub_081A4BF8);
}
