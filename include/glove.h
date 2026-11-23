#ifndef GUARD_GLOVE_H
#define GUARD_GLOVE_H
#include "global.h"
#include "scene_pool.h"
#include "scene_actor_style.h"
#include "spatial_entry.h"

/* The Glove controller (0811697C-0811A9xx): the player's glove actor, a 0x958-byte
 * object whose movement body sits at +0x18 and whose sprite model follows it.
 * Merged from the per-unit declarations of the units that drive it. Names are
 * tentative (confidence=low) unless a sibling confirms them. */

struct GloveModel;
struct Glove;

/* The animation key at +0x524, read as a word and as its two halves. */
union GloveAnimationKey {
    u32 word;
    u16 half[2];
};

/* The movement body, 0x5F8 bytes at Glove+0x18. */
struct GloveMotion {
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
    u8 color[4];
    struct SceneActorStyle style; /* 0x48 */
    u8 pad64[4];
    u32 pendingDamage; /* 0x68 */
    u8 pad6C[0x7E - 0x6C];
    u16 recoveryValue;
    struct GloveModel *parts; /* 0x80 */
    u8 pad84[0xB0 - 0x84];
    u8 drain[0x10]; /* 0xB0: drain modifier state */
    s32 unkC0;
    u8 padC4[4];
    u32 secondaryBias; /* 0xC8 */
    u8 padCC[0x2B0 - 0xCC];
    u32 primaryCallback; /* 0x2B0 */
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
    u32 secondaryCallback; /* 0x314 */
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
    union GloveAnimationKey animation; /* 0x524 */
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
    s8 damageFlags;
    u8 pad5EF;
    u16 angle; /* 0x5F0 */
    u8 pad5F2[2];
    struct Glove *owner; /* 0x5F4 */
};

struct GloveModelPart {
    u8 pad0[8];
    u32 flags;
    u8 padC[0x20 - 0xC];
    u8 drainSource[0x48 - 0x20]; /* 0x20 (guess, confidence=low) */
    void *palette;
    u8 pad4C[0x60 - 0x4C];
};

/* The sprite model, 0x250 bytes at Glove+0x610: five parts, then the primary
 * animation and the palette. */
struct GloveModel {
    struct GloveModelPart parts[5];
    u8 primaryAnimation[0x30];
    u8 palette[0x40];
};

/* The draw record at Glove+0x8C8. */
struct GloveDraw {
    u8 pad0[0x16];
    u16 flags;
};

struct Glove {
    u8 pad0[0x18];
    struct GloveMotion motion; /* 0x18 */
    struct GloveModel model;   /* 0x610 */
    s8 marker, markerSlot;     /* 0x860 */
    u8 stylePlayer;            /* 0x862 */
    u8 pad863;
    u32 timer;  /* 0x864 */
    u8 active;  /* 0x868 */
    u8 pad869[3];
    void (*update)(void *); /* 0x86C */
    u8 pad870[0x8A4 - 0x870];
    struct ScenePoolVec32 origin, scroll; /* 0x8A4 */
    u16 *input;                           /* 0x8C4 */
    struct GloveDraw draw;                /* 0x8C8 */
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
    u32 participantIndex; /* 0x91C */
    u8 pad920[0x30];
    u32 pending[2]; /* 0x950 */
};

typedef char GloveMotionSizeCheck[sizeof(struct GloveMotion) == 0x5F8 ? 1 : -1];
typedef char GloveModelSizeCheck[sizeof(struct GloveModel) == 0x250 ? 1 : -1];
typedef char GloveSizeCheck[sizeof(struct Glove) == 0x958 ? 1 : -1];

#endif
