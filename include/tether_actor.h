#ifndef GUARD_TETHER_ACTOR_H
#define GUARD_TETHER_ACTOR_H
#include "contact_swarm.h"
#include "engine.h"
#include "player.h"
struct TetherSegment {
    u32 flags;
    u8 unknown4[12];
    u8 priority, unknown11[7];
    struct ContactPosition position;
    u8 unknown20[8];
};
/* Tentative identity: world actor with seven visible player-tether segments. */
struct TetherPaletteView {
    u8 unknown0[0x18];
    const u16 *palette;
    u8 unknown1C[0x58];
    const u16 *keyframePalette;
    u8 unknown78[0x1C];
};
/* Twelve-byte body prefix; initialization also writes the following position/probe fields. */
struct TetherMotionDelta {
    s16 x, y, z;
    u16 unknown6; /* initialized to 16 by the body initializer */
} __attribute__((aligned(4)));
struct TetherBodyConfig {
    u32 id, flags;
    u8 category, angle, unknownA[2];
};
struct TetherMovementProbe {
    u8 unknown0[4];
    u16 previousCell, cell;
    u8 previousHeight, height, previousSlope, slope;
    u16 previousAttributes, attributes;
};
struct TetherInteractionProbe {
    u16 flags;
    u8 unknown2[0x1A];
    struct ContactPosition target;
    u16 bodyId;
    u8 unknown26[2];
    struct ContactPosition *position;
    u8 unknown2C[0x3C];
};
struct TetherActor {
    u8 unknown0[0x18];
    u16 entry;
    u8 kind, unknown1B;
    u16 kindValue, kindLimit, valueA, valueB;
    u16 palette[16];
    struct TetherBodyConfig bodyConfig;
    struct ContactPosition position;
    struct TetherMotionDelta movementDelta;
    void *movementProbe;
    u8 unknown64[0x28];
    union {
        struct DisplayAnimation animation;
        struct TetherPaletteView colors;
    } display;
    struct TetherMovementProbe movementProbeStorage;
    struct ContactCollider chargeCollider, secondaryCollider;
    struct TetherInteractionProbe interactionProbe;
    u8 actionRequested, actionAngle;
    u16 actionMagnitude;
    u8 auxiliaryRegistration[0x10];
    u8 auxiliaryRegistered, state;
    u16 stateFrame;
    struct ContactPosition savedPosition;
    u8 heading, unknown261[3];
    struct TetherSegment segments[7];
    u16 paletteIndex;
    s16 shakeX;
    u16 displayOffsetY;
    s16 shakeZ;
    u16 chargeReady, charge;
    u16 shakeFrame;
    u16 glowMode, glowFrame;
    u16 appliedPalette, paletteOverrideTimer, contactCooldown;
    u16 paletteBlend;
    u16 effectFrame, soundCooldown;
    u8 moving;
    u8 auxiliaryActive;
    struct ContactSwarmSprite auxiliarySprite;
    struct ContactSwarmGraphics auxiliaryGraphics;
    u8 statusFlags;
    u8 unknown3E5;
    u16 statusTag;
    u8 requestStatus, requestMode, unknown3EA[2];
    u32 onActionScriptId;
    struct ContactPosition targetPosition;
    u16 secondaryValue, secondaryMaximum;
    u16 interactionLock;
    u16 lockFrame;
    u16 recoveryTimer, recoveryDuration;
    u16 lockSoundFrame, transitionReason;
    s16 parameter;
    u16 interactionEnabled, currentValue, maximumValue;
    struct ContactPosition interactionTarget;
    u32 interactionScriptId;
    u16 interactionReset, interactionCooldown;
    u16 roamFrame, turnAfterBlock;
    struct ContactPosition directedTarget;
    struct ContactPosition origin;
    struct ContactPosition auxiliaryOrigin;
    /* State-entry tuning, tentative: initialized together at 0804AA08. */
    u16 stateOption, stateCounter, unknown440;
    u16 stateRadius, stateSpeed, stateDuration, stateRepeat, stateDelay;
    u16 stateRecovery, stateStep, statePhase;
    u16 stateLimitA, stateLimitB, stateFlagA, stateLimitC, stateFlagB;
    u16 stateLimitD, stateFlagC, stateLimitE, stateFlagD;
    u16 terrainCheckReady;
    u8 unknown466[4];
    u16 directedTimer, directedHeading;
    u16 blendHold;
    u16 directedStep;
    u8 blendRequested, destroyRequested;
    struct ContactPosition sampledPosition;
    void *pendingEffectHandle;
    u16 effectValue, unknown482;
};
/* 0x02000488 */
extern struct TetherActor *gTetherActor;
void ParticleSprite_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void Sprite_SetOriginStub(void *, s32, s32);

struct TetherPackedPair {
    u32 lo : 16;
    u32 hi : 16;
};
struct TetherColliderVector {
    struct TetherPackedPair a, b;
};
#endif
