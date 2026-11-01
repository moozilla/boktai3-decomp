#include "global.h"
#include "scene_actor_control_phase.h"

#define F(p, o, T) (*(T *)((u8 *)(p) + (o)))

static inline void Control(struct SceneActorRegistryResetView *a, void *c, u32 k, u32 z)
{
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = k;
    a->target = c;
    a->field2C8 = z;
    {
        u32 *p = &a->flags114;
        u32 m = ~1;
        *p &= m;
    }
    a->word104 = z;
}
static inline void State(struct SceneActorRegistryResetView *a, u32 k, u32 z)
{
    a->state2A8 = z;
    a->phase2A9 = z;
    a->behavior2AA = k;
    a->counter2AC = z;
    a->field2B1 = 1;
}

static inline u32 Actor16(struct SceneActorRegistryResetView *a, u32 m)
{
    return a->flags2D4 & m;
}
static inline u32 Actor32(struct SceneActorRegistryResetView *a, u32 m)
{
    return a->flags2D0 & m;
}
static inline u8 Disabled(struct SceneActorRegistryResetView *a)
{
    if (a) {
        if (Actor16(a, 64) || Actor32(a, 0x20000) || F(a, 0x9c, s16) <= 0)
            return 1;
    }
    return 0;
}
struct ContactPosition *TetherActor_GetPosition(void);
s32 Nav_IsPositionInMesh(void *, void *, void *);
s32 Terrain_ApplyBlocked(void *, void *, void *);
static inline void ClearBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}
static inline u8 Route(struct SceneActorRegistryResetView *a, void *nav, void *pos, void *target)
{
    s32 r;
    if (!Nav_IsPositionInMesh(nav, pos, target)) {
        ClearBits(&a->flags2D4, 0x200);
        return 0;
    }
    r = Terrain_ApplyBlocked(nav, pos, target);
    if (r) {
        if (r != 2)
            Memory_Clear((u8 *)a + 0x64, 8);
        return 1;
    }
    ClearBits(&a->flags2D4, 0x200);
    return 0;
}
s32 SceneActorPlacement_TriggerTetherAction(struct SceneActorRegistryResetView *a)
{
    u8 *c = (void *)SceneActorControl_GetState(a);
    u8 *nav = (u8 *)a + 0x3d4;
    struct ContactPosition *target;
    if (Disabled(a))
        return 0;
    target = TetherActor_GetPosition();
    if (!target)
        return 0;
    if (!Route(a, F(nav, 0x1fc, void *), &a->body.position, target))
        return 0;
    {
        struct Flags {
            s16 bits : 16;
        };
        struct Flags *p = (void *)(c + 0x62C);
        p->bits |= 4;
        Control(a, F(a, 0x228, void *), 1, 0);
        State(a, 7, 0);
    }
    a->field2C2 = 16;
    return 1;
}
