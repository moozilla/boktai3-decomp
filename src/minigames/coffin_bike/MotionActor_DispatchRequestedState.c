#include "global.h"
#include "engine/engine_pause.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "minigames/coffin_bike/coffin_bike.h"
void Hud_SetMode(s32);
void sub_08116E44(void);
void CoffinBike_UpdateDefeatEffects(void);
void CoffinBike_DispatchCommands(struct CoffinBike *);
void ActorStats_ApplyPendingIncrement(struct CoffinBikeMotion *);
void StageIconDisplay_Create(u32, u32, void (*)(void), void *);
static inline u8 TestActorFlags(struct CoffinBike *p, u32 mask)
{
    if (p->motion.damageFlags & mask)
        return 1;
    return 0;
}
static inline u8 TestStateFlags(struct CoffinBikeMotion *state, u32 mask, u8 **output)
{
    u8 *p = &state->damageFlags;
    u32 value = *p & mask;
    *output = p;
    if (value)
        return 1;
    return 0;
}
static inline u8 IsUpdated(struct CoffinBike *p)
{
    if (p) {
        u32 flags = p->flags;
        if (flags & 1)
            return 1;
    }
    return 0;
}
static inline void SetCallback(struct CoffinBike *p, void (*callback)(void), u32 zero)
{
    p->active = 1;
    p->update = (void (*)(void *))callback;
    p->timer = zero;
}
void MotionActor_DispatchRequestedState(struct CoffinBike *actor)
{
    struct CoffinBikeMotion *state;
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
            if (!Engine_PauseBits(0x800)) {
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
            SetCallback(actor, CoffinBike_UpdateDefeatEffects, zero);
            return;
        }
    }
    CoffinBike_DispatchCommands(actor);
    if (!IsUpdated(actor)) {
        ActorStats_ApplyPendingIncrement(state);
    }
}
