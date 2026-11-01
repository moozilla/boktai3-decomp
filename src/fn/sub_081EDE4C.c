#include "contact_swarm.h"
// confidence=low: moves two sprite sequences left before destruction.
#include "scene_vec.h"
#include "ram.h"
#include "player_cue_sequence.h"
s32 SceneMover_StepPastScreenX(void *, s32, s32);
extern const u16 gUnk_086141BC[];
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
void SceneActors_EmitPeriodicParticles(void *, s32);
void SceneActors_UpdateIdleDisplay(void *);
// confidence=low: departs past the left edge, then schedules destruction.
#define SceneActors_UpdatePastEdgeDeparture sub_081EDE4C
/* Tentative fields of the actor's departure display state. */
struct DepartureActor {
    u8 unknown0[0x90];
    s32 stateTimer;
    u8 unknown94[0xE];
    u8 phase, unknownA3;
    s32 phaseTimer;
    u8 unknownA8[9], entered;
    u8 unknownB2[0x4E];
    void (*callback)(void *);
    u8 unknown104[0x180];
    s32 verticalVelocity, randomOffset;
    u8 unknown28C[4];
    u32 field290;
    u8 unknown294[4];
    u32 field298;
    u8 unknown29C[0x24];
    struct Drawable foreground, accent;
    u8 unknown350[0x40];
    const void *sequenceResource;
};

static inline u32 RandomValue(void)
{
    return gUnk_086141BC[gEffectRandomTableIndex];
}

void SceneActors_UpdatePastEdgeDeparture(struct DepartureActor *input, s32 time)
{
    s32 result = 0;
    struct DepartureActor *p = input;
    u8 *enter;
    enter = &p->entered;
    if (*enter) {
        struct Drawable *foreground;
        struct Drawable *accent;
        struct DisplaySequenceState *state;
        *enter = result;
        foreground = &p->foreground;
        accent = &p->accent;
        p->field298 = result;
        p->field290 = result;
        foreground->unknown3C = result;
        state = &p->foreground.sequence;
        Anim_SetSequence(state, p->sequenceResource, 0, 0, result);
        PlayerCue_StepSequenceEnded(&foreground->sprite, state);
        accent->unknown3C = 4;
        Anim_SetSequence(&accent->sequence, p->sequenceResource, 4, 0, 0);
        PlayerCue_StepSequenceEnded(&accent->sprite, &accent->sequence);
    }
    result = SceneMover_StepPastScreenX(input, -40, 0x5000);
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    {
        u32 one;
        p->randomOffset = RandomValue() & (one = 1);
        ActorDisplay_UpdateScreenAnchor(input);
        ActorDisplay_UpdateForegroundSprite(input);
        ActorDisplay_UpdateAccentSprite(input);
        SceneActors_EmitPeriodicParticles(input, time);
        if (result) {
            {
                void (*callback)(void *) = SceneActors_UpdateIdleDisplay;
                input->callback = callback;
            }
            input->stateTimer = 0;
            input->entered = one;
            input->phase = 0;
            input->phaseTimer = 0;
            p->verticalVelocity = -16;
        }
    }
}

void sub_081EE12C(void)
{
}
