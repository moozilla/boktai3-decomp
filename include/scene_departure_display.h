#ifndef GUARD_SCENE_DEPARTURE_DISPLAY_H
#define GUARD_SCENE_DEPARTURE_DISPLAY_H

#include "player_cue_sequence.h"
#include "scene_pool.h"
#include "scene_vec.h"
#include "scene_event_dispatch.h"
#include "damage_number_display.h"

struct DepartureDrawable {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    u32 sequenceId;
    s32 ended;
    u32 mode;
};

struct DepartureActor {
    u8 unknown0[0x90];
    s32 stateTimer;
    s16 health;
    u8 unknown96[6];
    u32 actorFlags;
    u8 unknownA0[2], phase, unknownA3;
    s32 phaseTimer;
    u8 unknownA8[4], state, substate, statePhase, finished, stateEntered, entered;
    u8 unknownB2, motionMode, unknownB4[8];
    u32 motionRequest, effectRequest, stateCounter;
    u8 unknownC8[0x1C];
    u32 ticks;
    u8 unknownE8[0x18];
    void (*callback)(void *);
    s32 (*response)(void);
    u8 unknown108[0xC], messageType, unknown115[3];
    struct SceneEventReceiver messages;
    u8 unknown140[0x3C];
    u32 primaryFlags;
    u8 unknown180[0xA], contactActive, unknown18B[0x23];
    u16 contactTimer;
    u8 unknown1B0[0xC];
    u32 secondaryFlags;
    u8 unknown1C0[0x3C];
    struct DamageNumberSource damageSource;
    u8 unknown204[0x3C];
    u16 damageTimer;
    u8 unknown242[0x1E];
    struct SceneVec initialPosition;
    struct ScenePoolVec32 position;
    struct SceneVec screen;
    s32 velocity, acceleration, jitter, unknown28C, displacement, targetX, displacementTimer;
    u8 unknown29C[0x10];
    s32 fixedZ;
    u8 unknown2B0[0x10];
    struct DepartureDrawable foreground, accent;
    struct ContactSwarmGraphics foregroundGraphics, accentGraphics;
    u32 effectResource, effectGraphics;
    void *sequenceResource;
    u32 unknown394;
    s32 motionState;
    u32 unknown39C;
    s32 playerIndex;
    u32 unknown3A4;
    u16 flashPalette[16], foregroundPalette[16], accentPalette[16];
    u16 fadeFrom[4][16], fadeTo[4][16], savedPalettes[4][16];
    s32 setting;
    u32 unknown58C, hit;
    u8 unknown594[8];
    s32 emitted, dispersalTimer;
    u8 unknown5A4, effectEnabled;
    s16 effectStyle;
    s32 fadeFrame, fadeShift;
    u8 unknown5B0[0x10];
    s16 pendingDamage, slowdownTimer;
    s16 parameterA, parameterB, unknown5C8, parameterC, parameterD, unknown5CE;
    u16 invincibilityTimer, targetMode;
    u8 unknown5D4[4];
    s16 burstX, burstY, burstTimer;
    u16 unknown5DE;
    s16 shakeStrength, phaseTimer2;
    u16 alternateLayer;
    s16 shakeX, shakeZ, blinkFrame, blinkActive;
    u16 cameraActive, healthSeventh, healthTenth;
    u32 foregroundFrom, foregroundTo, foregroundBank, accentFrom, accentTo, accentBank;
    union {
        struct { u16 *from[4], *to[4]; } banks;
        u16 *rows[8];
    } paletteLinks;
    void *effectPool;
};

void ActorDisplay_UpdateScreenAnchor(void *);
void ActorDisplay_UpdateForegroundSprite(void *);
void ActorDisplay_UpdateAccentSprite(void *);
void SceneActors_EmitPeriodicParticles(void *, s32);

static inline void DepartureActor_EnterDisplay(struct DepartureActor *input)
{
    if (input->entered) {
        struct DepartureDrawable *foreground, *accent;
        struct DisplaySequenceState *state;
        input->entered = 0;
        foreground = &input->foreground;
        accent = &input->accent;
        input->displacementTimer = 0;
        input->displacement = 0;
        foreground->sequenceId = 0;
        state = &input->foreground.sequence;
        Anim_SetSequence(state, input->sequenceResource, 0, 0, 0);
        PlayerCue_StepSequenceEnded(&foreground->sprite, state);
        accent->sequenceId = 4;
        state = &accent->sequence;
        Anim_SetSequence(state, input->sequenceResource, 4, 0, 0);
        PlayerCue_StepSequenceEnded(&accent->sprite, &accent->sequence);
    }
}
#endif
