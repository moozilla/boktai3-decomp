#ifndef GUARD_SCENE_POOL_H
#define GUARD_SCENE_POOL_H
#include "engine.h"
#include "modifier_list.h"
#include "spatial_entry.h"

/* The copy helpers transfer 16 bytes; only x/y/z are interpreted.
 * The unnamed word is representation padding, not an uninitialized coordinate. */
struct ScenePoolVec32 {
    s32 x, y, z;
    u32 : 32;
};

/* Tentative pooled scene-motion controller, 0803CDF0 onward. Pool entries
 * occupy 0x17c bytes; the manager also owns four 0x8b0 actor slots.
 * State meanings remain provisional until the dispatcher's callers are mapped. */
struct ScenePoolEntry {
    u8 unknown0[0x0c];
    s32 scriptId;
    u32 index;
    struct ScenePoolVec32 position;
    u8 active, kind;
    u16 boundsFlags;
    s32 duration, parameter;
    u32 origin, limit;
    u8 side, mode, repetitions;
    u8 unknown3b;
    u32 unknown3c;
    s32 motionPhase; /* legacy name: target-player index, selected by 0803C114 */
    u32 stateFrame;
    s32 step, acceleration, speed, turnStep;
    s32 direction; /* legacy name: target-player x minus actor x (0803C114) */
    s32 unknown5c; /* target-player z minus actor z (0803C114) */
    s32 idleFrame;
    u8 stateEntered;
    u8 speedControlActive, speedControlDelay;
    s8 speedControlPulse;
    u32 state;
    u32 history[0x40]; /* per-state workspace; temporal history is not established */
    s32 settingA, delayA, settingB, delayB;
};
static inline void ScenePool_EnterStateAndClearHistory(struct ScenePoolEntry *entry, u32 state)
{
    entry->state = state;
    entry->stateFrame = 0;
    entry->stateEntered = 1;
    Memory_Clear(entry->history, sizeof(entry->history));
}

/* Drain dispatch prefix at +0xB0; accumulator shares forwardBias at +0xC0. */
struct ActorDrainState {
    u32 (*callback)(void *);
    void *resource;
    s16 timer, duration;
    s8 effect;
    u8 valueBase, entered, active;
};
/* Partial actor view established by the frame/state callbacks in this range. */
struct ScenePoolActor {
    u32 inputFlags;
    u32 unknown4;
    u32 kind;
    u32 boundsFlags;
    struct ScenePoolVec32 position;
    s32 motionX, motionY;
    s32 motionDisplacement; /* +0x28: tween coordinate correction */
    u8 unknown2c[0xc];
    s32 restrictionMetric;
    s32 terrainHeightB;
    u32 movementFlags;
    u8 colorEffect[4];
    u8 bodyAnimation, faceAnimation, accessoryAnimation, paletteState, paletteMode;
    u8 unknown4d[3];
    u8 sourceIds[4];
    u8 modifierCount;
    u8 unknown55[3];
    u16 timerA, timerB;
    s32 currentAmount, amountLimit, halfAmount;
    u32 unknown68;
    u8 unknown6c[0x18];
    s32 acceleration, turnAcceleration;
    u8 unknown8c[4];
    s32 drainIncrement;
    s32 minimumForwardSpeed, maximumForwardSpeed;
    s32 minimumLateralSpeed, maximumLateralSpeed;
    s32 minimumForwardBias, maximumForwardBias;
    u8 airborne;
    u8 unknownad[3];
    struct ActorDrainState drain;
    s32 forwardBias; /* +0xc0, added to forwardSpeed (tentative) */
    u8 unknownc4[4];
    s32 unknownc8; /* cleared with the speeds when the removal effects start */
    u8 unknowncc[4];
    struct StatModifier modifiers[24];
    u8 unknown2b0[4];
    u8 emissionProfile[0xc]; /* observed descriptor prefix; full extent is unknown */
    u8 unknown2c0[0x48];
    u16 unknown308;
    u8 unknown30a[0x30];
    u16 drainQuarter;
    s32 biasClampMode;
    u8 unknown340[0x1dc];
    u16 baseAmount;
    u8 unknown51e[6];
    u16 requestedSound, playingSound;
    u8 unknown528[4];
    struct SpatialEntry hitCollider;
    u32 motionFlags;
    u8 unknown570[0xa];
    u8 unknown57a;
    u8 unknown57b[0x31];
    s32 forwardSpeed, verticalSpeed, lateralSpeed;
    u8 unknown5b8[4];
    s32 previousX;
    u8 unknown5c0[4];
    s32 previousZ;
    u8 unknown5c8[4];
    s32 impulse[4];
    s32 steering;
    u8 unknown5e0[0xc];
    s8 motionLock;
    u8 unknown5ed;
    u8 flags5ee;
    u8 unknown5ef;
    u16 heading;
    u8 unknown5f2[2];
    struct ScenePoolPlayer *linkedPlayer; /* +0x5f4, restriction flag consumer 0811628C */
    u8 unknown5f8[4];
    u32 stateFlags;
    u8 model[0x250];
    s8 marker;
    u8 unknown851[3];
    s32 entryIndex;
    u32 controlFlags;
    s32 proximityCooldown;
    struct SpatialEntry proximityCollider;
    struct ScenePoolVec32 savedPosition;
};
struct ScenePoolManager {
    struct Task task;
    u32 capacity, activeCount;
    struct ScenePoolEntry *entries;
    u32 seenIds[8];
    struct ScenePoolActor actors[4];
    u32 playerMask;
    s32 playerCount;
    u32 regions[2];
    u32 leftBounds[2], rightBounds[2];
    s32 playerPositions[2][4];
    u32 configuration, currentConfiguration;
    u32 firstId, lastId;
    u32 registry[3];
};
/* Five signed thresholds used by the adjacent forward-speed callbacks. */
struct ScenePoolSpeedBounds {
    s32 emergencyHigh, high, low, activeHigh, activeLow;
};
/* Position and flag consumers identify this separate player-pointer array.
 * This is not the gPlayers Player array from player.h. */
struct ScenePoolPlayerDraw {
    u8 unknown0[0x16];
    u16 flags;
};
struct ScenePoolPlayer {
    u8 unknown0[0x28];
    s32 position[3];
    u8 unknown34[0x5c0];
    s32 viewportOriginX; /* +0x5f4, rounded down to a pixel by 0811AB78 */
    s32 viewportOriginY,
        viewportOriginZ; /* +0x5f8/+0x5fc: copied with viewportOriginX by emitter */
    u8 unknown600[0x264];
    u32 gloveTimer; /* +0x864, tentative Glove controller state */
    u8 gloveActive;
    u8 unknown869[3];
    void (*gloveUpdate)(void);
    u8 unknown870[0x58];
    struct ScenePoolPlayerDraw gloveDraw; /* +0x8c8 */
    u8 unknown8e0[0x14];
    u16 flags;
};
/* 0x020004F0 */
extern struct ScenePoolPlayer *gParticipantActors[2];

static inline void ScenePoolActor_SetStateFlags(struct ScenePoolActor *actor, u32 mask)
{
    actor->stateFlags |= mask;
}

/* Whether the first participant is present with any of MASK in its flags. */
static inline u8 ScenePool_PlayerHasFlag(u32 mask)
{
    if (gParticipantActors[0] && (gParticipantActors[0]->flags & mask))
        return 1;
    return 0;
}
#endif
