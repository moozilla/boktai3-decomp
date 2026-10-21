#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
#include "scene_actor_chase.h"

s32 Callback_InvokeActor(void *, void *);
void Crow_OnSpecialAnimation(void);

struct CrowKnockbackState {
    u8 unknown0[0x42E];
    s16 velocityY;
};

static inline u8 TakeEntry(struct SceneActorRegistryResetView *s)
{
    if (s->field2B1) {
        s->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void Control(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u8 *taken = &a->field2B2;
    u32 zero = 0;
    *taken = 1;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = zero;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = zero;
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
void Crow_UpdateKnockback(struct SceneActorRegistryResetView *actor)
{
    struct CrowKnockbackState *state = actor->context;
    u32 mask = 0x42;
    if (TakeEntry(actor)) {
        Control(actor, Crow_OnSpecialAnimation, 0x26);
        state->velocityY = 30;
    }
    if (((u16 *)SceneActorTerrain_GetCell(actor->terrainCell))[1] & mask) {
        if ((s32)actor->counter2AC > 20) {
            SetState(actor, 0, 0, 4);
            return;
        }
    } else {
        u32 mask2 = 0xA02F;
        u16 *speed;
        if (actor->flags2D4 & mask2) {
            u16 *t = &actor->field2C2;
            *t = 0x40;
            speed = t;
        } else {
            u16 *t = &actor->field2C2;
            u32 v = *t;
            speed = t;
            if (v == 0)
                *speed = 0x40;
        }
        if (actor->drawableKind == 0)
            Anim_SetDurationScale((u8 *)actor->drawable + 0x48, *speed);
        else
            Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, *speed);
        actor->field2EA += state->velocityY;
        if (Mod(actor->counter2AC, 4) == 0) {
            s32 v = state->velocityY;
            s32 h;
            if (v >= 0)
                h = v >> 1;
            else
                h = -(-v >> 1);
            state->velocityY = h;
        }
        if (state->velocityY <= 0) {
            SetState(actor, 0, 2, 1);
            return;
        }
    }
    actor->counter2AC++;
}
