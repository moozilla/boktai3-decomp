#ifndef GUARD_TETHER_ACTOR_H
#define GUARD_TETHER_ACTOR_H
#include "contact_swarm.h"
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
    u8 unknown0[0x24];
    u16 bodyId;
    u8 unknown26[0x42];
};
struct TetherActor {
    u8 unknown0[0x18];
    u16 entry;
    u8 kind, unknown1B;
    u16 kindValue, kindLimit, valueA, valueB;
    u16 palette[16];
    struct TetherBodyConfig bodyConfig;
    struct ContactPosition position;
    struct ContactPosition movementPosition;
    void *movementProbe;
    u8 unknown64[0x28];
    union {
        struct DisplayAnimation animation;
        struct TetherPaletteView colors;
    } display;
    struct TetherMovementProbe movementProbeStorage;
    struct ContactCollider chargeCollider, secondaryCollider;
    struct TetherInteractionProbe interactionProbe;
    u8 actionRequested, actionVariant;
    u16 actionValue;
    u8 auxiliaryRegistration[0x10];
    u8 auxiliaryRegistered, state;
    u16 stateFrame;
    struct ContactPosition savedPosition;
    u8 heading, unknown261[3];
    struct TetherSegment segments[7];
    u16 paletteIndex;
    s16 shakeX;
    u16 auxiliaryTimer;
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
    u8 requestStatus, requestMode, unknown3EA[6];
    struct ContactPosition targetPosition;
    u16 secondaryValue, secondaryMaximum;
    u16 interactionLock;
    u16 unknown3FE;
    u16 secondaryMode, secondaryTimer;
    u8 unknown404[4];
    s16 parameter;
    u16 interactionEnabled, currentValue, maximumValue;
    struct ContactPosition interactionTarget;
    u32 interactionValue;
    u16 interactionReset, interactionCooldown;
    u8 unknown420[4];
    struct ContactPosition directedTarget;
    struct ContactPosition origin;
    struct ContactPosition auxiliaryOrigin;
    u8 unknown43C[0x2E];
    u16 directedTimer, directedHeading;
    u16 blendHold;
    u16 directedStep;
    u8 blendRequested, sampleRequested;
    struct ContactPosition sampledPosition;
    void *pendingEffectHandle;
    u16 effectValue, unknown482;
};
/* Additional player view: these fields are distinct from player.h's facing/actionMode. */
struct TetherPlayer {
    u8 unknown0[0x30];
    struct ContactPosition position;
    u8 unknown38[0x36A];
    u8 heading, side;
    u8 unknown3A4[0xB3];
    u8 tetherMode;
};
extern struct TetherPlayer *gUnk_02000580[4];
extern const s16 gUnk_086149C4[];
extern u32 gUnk_03005308;
void sub_08217DE0(void *, void *, u32);
void sub_08217EC4(void *, s32, s32);
void sub_08214584(void *, s32, s32);
#endif
