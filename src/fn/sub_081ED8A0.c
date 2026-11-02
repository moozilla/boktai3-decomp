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

struct SceneMovementDisplay {
    u8 unknown0[0xAF];
    u8 finished, unknownB0, entered;
    u8 unknownB2[0x284 - 0xB2];
    s32 action, variation, unknown28C, unknown290, targetX, unknown298;
    u8 unknown29C[0x2C0 - 0x29C];
    struct Drawable foreground, accent;
    u8 unknown350[0x390 - 0x350];
    void *animation;
};
void ActorDisplay_UpdateScreenAnchor(void *);
void ActorDisplay_UpdateForegroundSprite(void *);
void ActorDisplay_UpdateAccentSprite(void *);
s32 SceneMover_StepPastScreenX(void *, s32, s32);
void SceneActors_EmitPeriodicParticles(void *, s32);
extern const u16 gUnk_086141BC[];
void SceneActors_UpdateDelayedMovement(u8 *p, s32 time)
{
    s32 savedTime;
    s32 completed;
    u8 *context;
    u8 *enter;
    struct SceneMovementDisplay *actor = (void *)p;
    struct SceneMovementDisplay *owner;
    savedTime = time;
    completed = 0;
    context = p;
    owner = (void *)context;
    enter = &actor->entered;
    if (*enter) {
        struct Drawable *foreground;
        struct Drawable *accent;
        struct DisplaySequenceState *state;
        u32 zero;
        zero = 0;
        *enter = zero;
        foreground = &actor->foreground;
        accent = &actor->accent;
        actor->unknown298 = zero;
        actor->unknown290 = zero;
        foreground->unknown3C = zero;
        state = &actor->foreground.sequence;
        Anim_SetSequence(state, actor->animation, zero, 0, zero);
        PlayerCue_StepSequence(&foreground->sprite, state);
        accent->unknown3C = 4;
        Anim_SetSequence(&accent->sequence, *(void **)(context + 0x390), 4, 0, 0);
        PlayerCue_StepSequence(&accent->sprite, &accent->sequence);
    }
    if (savedTime > 120)
        completed = SceneMover_StepPastScreenX(p, *(s32 *)(context + 0x294), 0x6000);
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    {
        u32 value = gUnk_086141BC[gEffectRandomTableIndex];
        u32 *offset = (u32 *)(context + 0x288);
        *offset = value & 1;
    }
    ActorDisplay_UpdateScreenAnchor(p);
    ActorDisplay_UpdateForegroundSprite(p);
    ActorDisplay_UpdateAccentSprite(p);
    SceneActors_EmitPeriodicParticles(p, savedTime);
    if (completed) {
        p[0xAF] = 1;
        *(u32 *)(context + 0x284) = 16;
    }
}
