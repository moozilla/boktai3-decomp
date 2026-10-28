#include "global.h"
#include "ram.h"
#include "engine_globals.h"
struct MotionActorState {
    u8 unknown0[0x68];
    u32 pending;
    u8 unknown6C[0x50E];
    u8 mode;
    u8 unknown57B[0x73];
    u8 flags;
};
struct MotionActor {
    u8 unknown0[0x18];
    struct MotionActorState state;
    u8 unknown608[0x25C];
    u32 scriptTimer;
    u8 scriptRequested;
    u8 unknown869[3];
    void (*callback)(void);
    u8 unknown870[0x84];
    u16 flags;
    u8 unknown8F6[0x16];
    u32 script;
};
void Hud_SetMode(s32);
void Script_Call(u32, void *);
void sub_08116E44(void);
void Glove_UpdateDefeatEffects(void);
void Glove_DispatchCommands(struct MotionActor *);
void ActorStats_ApplyPendingIncrement(struct MotionActorState *);
void StageIconDisplay_Create(u32, u32, void (*)(void), void *);
static inline u8 TestActorFlags(struct MotionActor *p, u32 mask)
{
    if (p->state.flags & mask)
        return 1;
    return 0;
}
static inline u8 TestStateFlags(struct MotionActorState *state, u32 mask, u8 **output)
{
    u8 *p = &state->flags;
    u32 value = *p & mask;
    *output = p;
    if (value)
        return 1;
    return 0;
}
static inline u8 IsUpdated(struct MotionActor *p)
{
    if (p) {
        u32 flags = p->flags;
        if (flags & 1)
            return 1;
    }
    return 0;
}
static inline void SetCallback(struct MotionActor *p, void (*callback)(void), u32 zero)
{
    p->scriptRequested = 1;
    p->callback = callback;
    p->scriptTimer = zero;
}
void MotionActor_DispatchRequestedState(struct MotionActor *actor)
{
    struct MotionActorState *state;
    state = &actor->state;
    actor->flags = 0;
    if (TestActorFlags(actor, 4)) {
        u8 *flags = &state->flags;
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
        state->mode = zero;
        state->pending = zero;
    } else {
        u16 testMask = 2;
        u8 *flags;
        if (TestStateFlags(state, testMask, &flags)) {
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
