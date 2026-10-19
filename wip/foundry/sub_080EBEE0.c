#define SceneActorReaction_BeginDelayedRestore sub_080EBEE0
/* Tentative name based on the observed state transition or initialization. */
#include "global.h"
#include "scene_actor_reset.h"
void sub_080EAA48(void);
void SceneActorReaction_RestoreAlternateAfterDelay(void);
void SceneActorTerrain_ResumeDrawable(void *);
void sub_080EBEE0(struct SceneActorRegistryResetView *actor)
{
    u32 state = 3;
    u32 zero;
    u32 one;
    u8 *group;
    u32 initialOffset = 0x2A8;
    u8 *field = (u8 *)actor + initialOffset;
    void *delayedCallback;
    zero = 0;
    *field = state;
    initialOffset++;
    *((u8 *)actor + initialOffset) = zero;
    initialOffset++;
    *((u8 *)actor + initialOffset) = zero;
    initialOffset += 2;
    *(u32 *)((u8 *)actor + initialOffset) = zero;
    initialOffset += 5;
    field = (u8 *)actor + initialOffset;
    one = 1;
    *field = one;
    group = (u8 *)actor + 0x460;
    delayedCallback = sub_080EAA48;
    *(u32 *)(group + 12) = 60;
    *(void **)(group + 16) = delayedCallback;
    group[8] = state;
    group[9] = zero;
    group[10] = zero;
    {
        void *callback = SceneActorReaction_RestoreAlternateAfterDelay;
        u32 kind = 14;
        u32 offset;
        offset = 0x2B2;
        *((u8 *)actor + offset) = one;
        offset -= 2;
        *((u8 *)actor + offset) = zero;
        offset += 5;
        *((u8 *)actor + offset) = kind;
        actor->target = callback;
        offset += 0x13;
        *(u16 *)((u8 *)actor + offset) = zero;
        {
            u32 *flags = &actor->flags114;
            u32 clear = ~1;
            *flags &= clear;
        }
        actor->word104 = zero;
    }
    SceneActorTerrain_ResumeDrawable(actor);
    if (!actor->drawableKind) {
        u32 *flags = actor->drawable;
        *flags |= one;
    } else {
        u8 *tile = (u8 *)actor->drawable + 0x20;
        *(u32 *)(tile + 8) |= one;
    }
}
