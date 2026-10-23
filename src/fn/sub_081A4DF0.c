#include "global.h"

// confidence=low: loads the projectile resource and initializes sixteen linked widgets.
// MATCHING_CONCESSION: register-pinning
#define Hresvelgr_InitializeProjectilePool sub_081A4DF0
struct ProjectileResource {
    u32 words[8];
};
struct ProjectilePosition {
    s16 x, y, z, pad;
};
struct ProjectileEntry {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0xD0];
    void *callback;
    u8 unknownE0[4];
    u16 timer, delay;
};
struct ProjectilePool {
    u8 unknown0[0x18];
    struct ProjectileResource resource;
    u8 unknown38[0x24];
    s16 parameterA, parameterB, parameterC;
    u16 active;
    struct ProjectileEntry entries[16];
};
struct ProjectileResource *ScriptProc_Start(u32, u32);
s32 ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
s32 Anim_SetKeyframes(void *, void *, u32, u32, u32);
static inline void ClearPosition(u32 *zero, struct ProjectilePosition *position)
{
    *zero = 0;
    CpuSet(zero, position, 0x05000002);
}
s32 Hresvelgr_InitializeProjectilePool(struct ProjectilePool *pool, s32 a, s32 b, s32 c)
{
    struct ProjectileResource *resource = ScriptProc_Start(0xCB05, 0xF771);
    u32 zero;
    struct ProjectilePosition position;
    struct ProjectileEntry *entry;
    s32 i;
    register s32 result asm("r0");
    if (!resource)
        return -1;
    pool->resource = *resource;
    ResourceHeader_Relocate(&pool->resource, resource);
    pool->parameterA = a;
    pool->parameterB = b;
    pool->parameterC = c;
    ClearPosition(&zero, &position);
    entry = pool->entries;
    for (i = 0; i < 16; i++) {
        Widget_InitLinked(entry, &pool->resource, 0, 0, 2, 0, 60, &position);
        result = Anim_SetKeyframes(entry, &pool->resource, 33, 2, 0);
        entry->flags |= 1;
        entry->timer = 0;
        entry->delay = 0;
        entry->callback = NULL;
        entry++;
    }
    pool->active = 0;
    asm("" : "+r"(result));
    return result;
}
