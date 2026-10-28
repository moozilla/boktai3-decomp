#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"
#include "engine.h"
#include "ram.h"

// MATCHING_CONCESSION: register-pinning

extern const s16 gUnk_086149C4[];
void Lun_OnSpecialAnimation(void);
void SceneActorControl_ConsumeLiftFlagPenalty(struct SceneActorRegistryResetView *);
#define AT(base, off, rc, ra)                                                                      \
    ({                                                                                             \
        register u32 _o asm(rc) = (off);                                                           \
        register u8 *_a asm(ra);                                                                   \
        asm("" : "+r"(_o));                                                                        \
        _a = (u8 *)(base) + _o;                                                                    \
        _a;                                                                                        \
    })
static inline u32 Random(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    {
        register u32 tab asm("r2") = (u32)gRandomTable;
        return *(const u16 *)(offset + tab);
    }
}
static inline u32 *ClearFlags(u32 *field, u32 mask)
{
    *field &= mask;
    return field;
}
static inline s32 Scale12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
static inline void ClearVector(u32 *zero, struct ContactPosition *v)
{
    *zero = 0;
    CpuSet(zero, v, 0x05000002);
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
// confidence=low: initializes a random launch vector and ends the action on motion flag 0x10.
void Lun_UpdateLaunchedReaction(struct SceneActorRegistryResetView *actor)
{
    u8 *state = actor->context;
    u32 zero;
    u8 entered = SceneActorControl_TakeFlag(&actor->field2B1);
    u32 *flags = &actor->flags2D0;
    if (entered) {
        u8 angle;
        SetControl(actor, Lun_OnSpecialAnimation, 0x24);
        SceneActorControl_ConsumeLiftFlagPenalty(actor);
        angle = Random() & 255;
        ClearVector(&zero, (struct ContactPosition *)AT(state, 0x424, "r3", "r1"));
        actor->secondaryMotion.x = Scale12(gUnk_086149C4[angle] * 12);
        actor->secondaryMotion.y = 10;
        actor->secondaryMotion.z = Scale12(gUnk_086149C4[(angle + 64) & 255] * 12);
        flags = ClearFlags(&actor->flags2D0, ~0x100000);
    }
    {
        u32 mask = 0x2000;
        actor->flags2CC |= mask;
    }
    {
        u32 mask = 0x10;
        if (*flags & mask) {
            u32 value = 0x100000;
            *flags |= value;
            ClearVector(&zero, &actor->secondaryMotion);
            SetState(actor, 0, 0, 4);
            SetControl(actor, SceneActorControl_OnIdle, 16);
        }
    }
    actor->counter2AC++;
}
