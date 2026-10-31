#ifndef GUARD_PLAYER_CUE_H
#define GUARD_PLAYER_CUE_H

#include "player.h"
#include "contact_swarm.h"
#include "display_sequence.h"
#include "script_utility.h"
#include "scene_event_dispatch.h"

/* Cue-specific views of fields in the player actor's otherwise unknown spans.
 * The owner remains struct Player; these records share the existing sprite,
 * playback and vector types. Names are tentative where a field has several
 * state-dependent uses. */
struct PlayerCueGraphics {
    u8 unknown0[0x6C];
    struct DisplaySequenceState primarySequence;
    u8 unknown7C[0x10];
    struct DisplaySequenceSprite primarySprite;
    u8 unknownB8[0x6E];
    u16 secondarySpriteResource;
    u8 unknown128[0xC];
    u32 secondaryVariantResource;
    u8 unknown138[0x14];
    struct ContactSwarmSprite sprite;
    u8 unknown178[0x284];
    struct DisplaySequenceState sequence;
    const void *setupResource, *sideResource, *alternateResource;
    u8 unknown418[0x53];
    u8 sequenceSelection;
    u8 unknown46C[0x7A];
    u16 visualOffsetStart, verticalScale, visualOffsetEnd;
};

struct PlayerCueAuxiliary {
    u8 unknown0[0x1D0];
    struct ContactSwarmSprite sprite;
    u8 unknown1FC[0x1C];
    struct DisplaySequenceState sequence;
    u32 sequenceEnded;
    const void *resource;
};

struct PlayerCueMotion {
    u8 unknown0[0x590];
    struct SceneVec destination;
    u8 unknown598[0x4F0];
    u32 particlePool;
    u8 unknownA8C[0x12];
    u16 speed;
    u8 unknownAA0[0x44];
    u16 overlayMode;
};

struct PlayerCueTimers {
    u8 unknown0[0x528];
    u16 remaining, elapsed, required;
};

struct PlayerCueMessages {
    u8 unknown0[0x24];
    u32 receiverIdentifier;
    u8 unknown28[0x2D0];
    struct SceneEventReceiver receiver;
    u8 unknown320[0x27E];
    u8 completionKind, acknowledgementPending;
    u8 unknown5A0;
    u8 periodicParticlesEnabled, periodicParticleFrame;
    u8 unknown5A3;
    u32 completionScript;
};

/* Word-aligned coordinate records used by cue effects and bulk copies. */
struct PlayerCueCoordinates {
    u8 unknown0[0x30];
    struct ContactPosition position;
    u8 unknown38[0x74];
    struct ContactPosition previousPosition;
    u8 unknownB4[0x58];
    struct ContactPosition alternatePosition;
    u8 unknown114[0x47C];
    struct ContactPosition destination;
    u8 unknown598[0x4F4];
    struct ContactPosition movementStart, movementDestination;
};

#define PlayerCue_Graphics(player)    ((struct PlayerCueGraphics *)(player))
#define PlayerCue_Auxiliary(player)   ((struct PlayerCueAuxiliary *)(player))
#define PlayerCue_Motion(player)      ((struct PlayerCueMotion *)(player))
#define PlayerCue_Timers(player)      ((struct PlayerCueTimers *)(player))
#define PlayerCue_Messages(player)    ((struct PlayerCueMessages *)(player))
#define PlayerCue_Coordinates(player) ((struct PlayerCueCoordinates *)(player))

typedef char PlayerCueGraphicsSizeCheck[(sizeof(struct PlayerCueGraphics) == 0x4EC) ? 1 : -1];
typedef char PlayerCueAuxiliarySizeCheck[(sizeof(struct PlayerCueAuxiliary) == 0x230) ? 1 : -1];
typedef char PlayerCueMotionSizeCheck[(sizeof(struct PlayerCueMotion) == 0xAE8) ? 1 : -1];
typedef char PlayerCueTimersSizeCheck[(sizeof(struct PlayerCueTimers) == 0x530) ? 1 : -1];
typedef char PlayerCueMessagesSizeCheck[(sizeof(struct PlayerCueMessages) == 0x5A8) ? 1 : -1];
typedef char PlayerCueCoordinatesSizeCheck[(sizeof(struct PlayerCueCoordinates) == 0xA9C) ? 1 : -1];

#endif
