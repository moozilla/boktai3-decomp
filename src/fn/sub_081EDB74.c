#include "global.h"

#include "contact_swarm.h"
#include "ram.h"
#include "player_cue_sequence.h"

// Drawable layout shared with ActorDisplay_UpdateForegroundSprite.
struct Drawable {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    u32 unknown3C;
    s32 ended;
    u32 mode;
};

static inline void SetCallback(void (*callback)(void *), void (**slot)(void *))
{
    *slot = callback;
}

void ActorDisplay_UpdateScreenAnchor(void *);
void ActorDisplay_UpdateForegroundSprite(void *);
void ActorDisplay_UpdateAccentSprite(void *);
void SceneActors_EmitPeriodicParticles(void *, s32);
void SceneActors_DestroyTask(void *);
// confidence=low: moves two sprite sequences left before destruction.
void SceneActors_UpdateLeftDeparture(u8 *input, s32 time)
{
    u8 *p = input;
    s32 savedTime;
    u8 *context;
    u8 *enter;
    savedTime = time;
    context = p;
    enter = p + 0xB1;
    if (*enter) {
        struct Drawable *foreground;
        struct Drawable *accent;
        struct DisplaySequenceState *state;
        u32 zero;
        zero = 0;
        *enter = zero;
        foreground = (struct Drawable *)(p + 0x2C0);
        accent = (struct Drawable *)(p + 0x308);
        *(u32 *)(p + 0x298) = zero;
        *(u32 *)(p + 0x290) = zero;
        foreground->unknown3C = zero;
        state = (struct DisplaySequenceState *)(p + 0x2EC);
        Anim_SetSequence(state, *(void **)(p + 0x390), zero, 0, zero);
        PlayerCue_StepSequenceEnded(&foreground->sprite, state);
        accent->unknown3C = 4;
        Anim_SetSequence(&accent->sequence, *(void **)(context + 0x390), 4, 0, 0);
        PlayerCue_StepSequenceEnded(&accent->sprite, &accent->sequence);
    }
    *(s32 *)(context + 0x280) = -0x5000;
    *(s32 *)(context + 0x284) = 0;
    *(s32 *)(context + 0x268) += *(s32 *)(context + 0x280);
    if (*(s16 *)(context + 0x278) < 0) {
        SetCallback(SceneActors_DestroyTask, (void (**)(void *))(p + 0x100));
        *(s32 *)(p + 0x90) = 0;
        *(p + 0xb1) = 1;
        *(p + 0xa2) = 0;
        *(s32 *)(p + 0xa4) = 0;
    }
    ActorDisplay_UpdateScreenAnchor(p);
    ActorDisplay_UpdateForegroundSprite(p);
    ActorDisplay_UpdateAccentSprite(p);
    SceneActors_EmitPeriodicParticles(p, savedTime);
}
