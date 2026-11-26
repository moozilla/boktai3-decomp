#include "engine/shared/global.h"
#include "engine/shared/ram.h"
#include "engine/shared/engine_globals.h"
#include "monsters/glove/glove.h"
void Hud_SetMode(s32);
void sub_08116E44(void);
void Glove_UpdateDefeatEffects(void);
void Glove_DispatchCommands(struct Glove *);
void ActorStats_ApplyPendingIncrement(struct GloveMotion *);
void StageIconDisplay_Create(u32, u32, void (*)(void), void *);
static inline u8 TestActorFlags(struct Glove *p, u32 mask)
{
    if (p->motion.damageFlags & mask)
        return 1;
    return 0;
}
static inline u8 TestStateFlags(struct GloveMotion *state, u32 mask, u8 **output)
{
    u8 *p = &state->damageFlags;
    u32 value = *p & mask;
    *output = p;
    if (value)
        return 1;
    return 0;
}
static inline u8 IsUpdated(struct Glove *p)
{
    if (p) {
        u32 flags = p->flags;
        if (flags & 1)
            return 1;
    }
    return 0;
}
static inline void SetCallback(struct Glove *p, void (*callback)(void), u32 zero)
{
    p->active = 1;
    p->update = (void (*)(void *))callback;
    p->timer = zero;
}
void MotionActor_DispatchRequestedState(struct Glove *actor)
{
    struct GloveMotion *state;
    state = &actor->motion;
    actor->flags = 0;
    if (TestActorFlags(actor, 4)) {
        u8 *flags = &state->damageFlags;
        s32 mask = ~7;
        u16 zero;
        mask &= *flags;
        do {
            zero = 0;
        } while (0);
        *flags = mask;
        SetCallback(actor, sub_08116E44, zero);
        {
            u32 mask = 0x800;
            if (!((gEngineFlags | gScenePauseFlags) & mask)) {
                StageIconDisplay_Create(1, 0, 0, actor);
            }
        }
        state->damageTimer = zero;
        state->pendingDamage = zero;
    } else {
        u8 *flags;
        if (TestStateFlags(state, 2, &flags)) {
            s32 mask = ~2;
            u16 zero;
            mask &= *flags;
            zero = 0;
            *flags = mask;
            SetCallback(actor, Glove_UpdateDefeatEffects, zero);
            return;
        }
    }
    Glove_DispatchCommands(actor);
    if (!IsUpdated(actor)) {
        ActorStats_ApplyPendingIncrement(state);
    }
}
