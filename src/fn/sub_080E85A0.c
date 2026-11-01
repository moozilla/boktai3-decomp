#include "global.h"
#include "scene_actor_reaction.h"
#include "engine.h"
void Sound_PlaySong(u32);

static inline u8 HasMode(u8 *p)
{
    u32 mask = 1;
    if (*(u16 *)(p + 0x65C) & mask)
        return 1;
    return 0;
}

static inline void SetControl(struct SceneActorRegistryResetView *actor, u32 k, void *control)
{
    u32 kind = k;
    u32 *flags;
    u32 mask;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    flags = &actor->flags114;
    mask = ~1;
    *flags &= mask;
    actor->word104 = 0;
}
static inline void ResetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void ClearBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}
u8 Mimic_StartHitReaction(struct SceneActorRegistryResetView *actor)
{
    u8 *ctx = actor->context;
    if (actor->reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x13E);
    if (actor->field2DC & 0xC0000) {
        SetControl(actor, 0x12, actor->reactionTarget230);
        ResetState(actor, 3, 0);
        return 1;
    }
    ClearBits(&actor->flags2D4, 0x20);
    if (actor->tracking.target) {
        if (HasMode(actor->context)) {
            {
                u16 mask = 0x80;
                u16 *flags = &actor->flags2D6;
                *flags = mask | *flags;
            }
            ClearBits((u16 *)((u8 *)actor->context + 0x65C), 1);
        } else if (actor->health < *(s32 *)(ctx + 0x660)) {
            {
                u16 mask = 0x80;
                u16 *flags = &actor->flags2D6;
                *flags = mask | *flags;
            }
            ClearBits((u16 *)((u8 *)actor->context + 0x65C), 1);
        }
    }
    return 0;
}
