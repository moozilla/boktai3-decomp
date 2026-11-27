#ifndef GUARD_COFFIN_BIKE_H
#define GUARD_COFFIN_BIKE_H
#include "scene/actor/modifier_list.h"
#include "global.h"
#include "link/wireless/scene_pool.h"
#include "scene/actor/scene_actor_style.h"
#include "scene/collision/spatial_entry.h"

/* The CoffinBike controller (0811697C-0811A9xx): the player's glove actor, a 0x968-byte
 * object whose movement body sits at +0x18 and whose sprite model follows it.
 * Merged from the per-unit declarations of the units that drive it. Names are
 * tentative (confidence=low) unless a sibling confirms them. */

struct CoffinBike;

/* The animation key at +0x524, read as a word and as its two halves. */
union CoffinBikeAnimationKey {
    u32 word;
    u16 half[2];
};

/* The particle emitter request at motion+0x44: a recoil command sets the kind
 * and type and raises `enter`; the low-resource color effect reads it too. */
struct CoffinBikeEmitter {
    u8 kind, unknown1, type, enter;
};

/* The movement body, 0x5F8 bytes at CoffinBike+0x18. */
struct CoffinBikeMotion {
    u8 pad0[4];
    u32 unk4;
    u32 unk8;
    u32 boundsFlags;
    struct ScenePoolVec32 position; /* 0x10 */
    s32 outX, outY, outZ;           /* 0x20 */
    u8 pad2C[0x38 - 0x2C];
    s32 unk38;
    s32 unk3C;
    u32 flags40;
    struct CoffinBikeEmitter emitter; /* 0x44 */
    struct SceneActorStyle style; /* 0x48 */
    u8 pad64[4];
    u32 pendingDamage; /* 0x68 */
    u8 pad6C[0x7E - 0x6C];
    u16 recoveryValue;
    void *parts; /* 0x80: the glove's model, read through a slot view */
    u8 pad84[0xB0 - 0x84];
    u8 drain[0x10]; /* 0xB0: drain modifier state */
    s32 unkC0;
    u8 padC4[4];
    u32 secondaryBias; /* 0xC8 */
    u8 padCC[0x2B0 - 0xCC];
    void *primaryCallback; /* 0x2B0 */
    u8 pad2B4, primaryKind;
    u8 pad2B6[4];
    u16 primaryValue; /* 0x2BA */
    u8 pad2BC[2];
    u16 primaryCharge; /* 0x2BE */
    u8 pad2C0[0x305 - 0x2C0];
    u8 primarySpriteRefresh, primaryLatched;
    u8 actionActive;     /* 0x307 */
    u16 actionCooldown;  /* 0x308 */
    u8 pad30A[4];
    u16 primaryKeyframes; /* 0x30E */
    u8 pad310[4];
    void *secondaryCallback; /* 0x314 */
    u8 pad318;
    u8 actionKind;
    u8 pad31A[4];
    u16 fourthValue; /* 0x31E */
    u8 pad320[4];
    u8 secondaryEnabled;
    s8 secondarySpriteSlot;
    u16 recoveryTimer, cooldown; /* 0x326 */
    u8 secondaryRelease, secondaryLatched;
    u8 pad32C[0x524 - 0x32C];
    union CoffinBikeAnimationKey animation; /* 0x524 */
    u8 pad528[4];
    struct SpatialEntry contact; /* 0x52C */
    u32 motionFlags;             /* 0x56C */
    u8 pad570[10];
    u8 damageTimer; /* 0x57A */
    u8 pad57B[0x59E - 0x57B];
    u16 spatialTimer; /* 0x59E */
    u8 pad5A0[0x5AC - 0x5A0];
    s32 vy; /* 0x5AC */
    s32 y;  /* 0x5B0 */
    s32 vz; /* 0x5B4 */
    u8 pad5B8[4];
    struct ScenePoolVec32 previousPosition; /* 0x5BC */
    s32 dx; /* 0x5CC */
    s32 dy; /* 0x5D0 */
    s32 dz; /* 0x5D4 */
    u8 pad5D8[4];
    s32 x; /* 0x5DC */
    u8 pad5E0[0x5EC - 0x5E0];
    s8 steps; /* 0x5EC */
    u8 pad5ED;
    u8 damageFlags;
    u8 pad5EF;
    u16 angle; /* 0x5F0 */
    u8 pad5F2[2];
    struct CoffinBike *owner; /* 0x5F4 */
};

/* A queued scene command for the glove (kind in `type`). */
struct CoffinBikeCommand {
    u8 unknown0[6], type, unknown7;
    s16 x, y;
    u16 c, d, e, f;
};

/* The command queue at CoffinBike+0x870, double-buffered by the dispatch bank. */
struct CoffinBikeQueue {
    u8 unknown0[6], count[2];
    struct CoffinBikeCommand *commands[2][4];
};

/* The recoil record at CoffinBike+0x8C8 (bit 0 of flags: the glove is active). A
 * recoil command writes its target here. confidence=low */
struct CoffinBikeDraw {
    s32 x, y, z;
    u8 unknownC[4];
    u8 b, d, a, c;
    u8 unknown14[2];
    u16 flags;
};

/* The attachment descriptor at CoffinBike+0x91C; its resource is the participant index. */
struct CoffinBikeAttachment {
    u32 resource, flags, unknown8;
};

struct CoffinBikePosition {
    s16 x, y, z;
};

struct CoffinBike {
    u8 pad0[0x18];
    struct CoffinBikeMotion motion; /* 0x18 */
    struct CharacterPreviewModel model; /* 0x610 */
    u32 timer;  /* 0x864 */
    u8 active;  /* 0x868 */
    u8 pad869[3];
    void (*update)(void *); /* 0x86C */
    struct CoffinBikeQueue queue; /* 0x870 */
    u8 pad898[0x8A4 - 0x898];
    struct ScenePoolVec32 origin, scroll; /* 0x8A4 */
    u16 *input;                           /* 0x8C4 */
    struct CoffinBikeDraw draw;                /* 0x8C8 */
    u8 pad8E0[4];
    u32 previousInput; /* 0x8E4 */
    s32 height;        /* 0x8E8 */
    u8 pad8EC[8];
    u16 flags, blinkTimer; /* 0x8F4 */
    s8 participant;        /* 0x8F8 */
    u8 pad8F9[3];
    s32 resourceFraction; /* 0x8FC */
    u32 activeFrames;     /* 0x900 */
    u32 localScript, entryScript, eventScript, nextScript; /* 0x904 */
    s32 launchSpeed; /* 0x914 */
    u8 launchPhase;  /* 0x918 */
    u8 pad919[3];
    struct CoffinBikeAttachment attachmentDesc; /* 0x91C */
    struct CoffinBikePosition screenPosition;   /* 0x928 */
    u8 pad930[0x950 - 0x930];
    s16 offsetX, offsetY; /* 0x950 */
    u8 pad954[0x964 - 0x954];
    void *attachment; /* 0x964 */
};

typedef char CoffinBikeMotionSizeCheck[sizeof(struct CoffinBikeMotion) == 0x5F8 ? 1 : -1];
typedef char CoffinBikeModelSizeCheck[sizeof(struct CharacterPreviewModel) == 0x254 ? 1 : -1];
typedef char CoffinBikeSizeCheck[sizeof(struct CoffinBike) == 0x968 ? 1 : -1];

#endif
