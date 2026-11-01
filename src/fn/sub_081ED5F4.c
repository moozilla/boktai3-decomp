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

void ActorDisplay_UpdateScreenAnchor(void *);
void ActorDisplay_UpdateForegroundSprite(void *);
void ActorDisplay_UpdateAccentSprite(void *);
void ActorMotion_AdvanceTowardScreenTarget(void *, s32);
void SceneActors_EmitPeriodicParticles(void *, void *);
extern const u16 gUnk_086141BC[];
// confidence=low: called by SceneActors_EnterMovementState.
void SceneActors_UpdateMovingDisplay(u8 *input, void *particles)
{
    u8 *p = input;
    void *savedParticles;
    u8 *context;
    u8 *enter;
    savedParticles = particles;
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
    ActorMotion_AdvanceTowardScreenTarget(p, *(s32 *)(context + 0x294));
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    {
        u32 value = gUnk_086141BC[gEffectRandomTableIndex];
        u32 *offset = (u32 *)(context + 0x288);
        *offset = value & 1;
    }
    ActorDisplay_UpdateScreenAnchor(p);
    ActorDisplay_UpdateForegroundSprite(p);
    ActorDisplay_UpdateAccentSprite(p);
    SceneActors_EmitPeriodicParticles(p, savedParticles);
}
