#ifndef GUARD_RATATOSK_H
#define GUARD_RATATOSK_H

#include "global.h"
#include "camera_tracking.h"
#include "contact_collider.h"
#include "contact_position.h"
#include "scene_actor_components.h"

/* The Ratatosk boss body (0xF30 bytes, Ratatosk_InitializeFromScript). Its first 0x178
 * bytes are the StateActor head (names as in include/state_actor.h); past that it has its
 * own layout. Merged from the per-file copies of the Ratatosk states; field meanings are
 * provisional (confidence=low) unless a comment says where they come from. */

/* A sun-orb point (0xD18) or target slot (0xDF0). */
struct RatatoskSlot {
    s32 active;
    struct ContactPosition position;
    u8 unknownC[4];
    u32 distance; /* 0x10: tested against 0x3FFF by the sun-orb player transfer. */
    u8 unknown14[4];
};

/* 0x3AC: motion interpolation whose current point is at +0x20 (Math_SetMotionPosition,
 * PolarInterpolation_SetCurrentFromPoint). */
struct RatatoskMotion {
    u8 unknown0[0x20];
    struct ContactPosition position;
    u8 unknown28[8];
};

struct Player;

struct Ratatosk {
    u8 unknown0[0x48];
    struct SceneActorBody body; /* 0x48: direction (0x51) gives facing and facing8. */
    u32 callbackFrame;          /* 0x90 */
    s16 health, maxHealth;
    u8 unknown98[2];
    u8 vulnerable; /* 0x9A */
    u8 unknown9B;
    u32 flags; /* 0x9C: StateActor actionFlags. */
    u8 unknownA0[2];
    u8 reactionPhase; /* 0xA2 */
    u8 animationCompletions;
    u32 reactionTimer; /* 0xA4 */
    u8 facing, facing8;
    u8 actionAnimation; /* 0xAA */
    u8 actionState;
    u8 actionMode, actionIndex, actionPhase; /* 0xAC */
    u8 actionFinished;                       /* 0xAF */
    u8 actionEnter;                          /* 0xB0 */
    u8 actionChanged;                        /* 0xB1: restarts the directional animation. */
    u8 unknownB2;
    u8 locked; /* 0xB3: keeps savedAngle and the current animation. */
    u8 unknownB4[8];
    u32 contactAttack, contactFlags; /* 0xBC: cleared every frame by the collider update. */
    u32 actionTimer;                 /* 0xC4 */
    u8 unknownC8[4];
    u8 requestedAngle; /* 0xCC */
    u8 unknownCD[3];
    struct ContactPosition targetPosition; /* 0xD0 */
    u8 unknownD8[0xE2 - 0xD8];
    u8 healthRatio;
    u8 unknownE3[5];
    u32 firstEventScript, secondEventScript; /* 0xE8 */
    u8 unknownF0[0x100 - 0xF0];
    void (*actionCallback)(void);
    void *actionTryUpdate; /* 0x104: called through Callback_InvokeActor. */
    void *actionReset;     /* 0x108 */
    struct Player *player; /* 0x10C: the target player. */
    const struct ContactPosition *playerPosition;
    s8 savedAngle; /* 0x114 */
    u8 unknown115[3];
    u8 receiver2[0x10]; /* 0x118: StateActor receiver2. */
    u8 unknown128[0x17C - 0x128];
    u32 actionCounter; /* 0x17C: frames or events counted by the current action. */
    s32 sequence;      /* 0x180: step of the current action's sequence. */
    u32 sequenceDone;  /* 0x184 */
    struct ContactCollider hurtCollider;     /* 0x188: Ratatosk_InitHurtCollider. */
    struct ContactCollider bodyCollider;     /* 0x1DC: Ratatosk_InitBodyCollider. */
    struct ContactCollider frontCollider;    /* 0x230: Ratatosk_InitFrontCollider. */
    struct ContactCollider outerCollider;    /* 0x284: Ratatosk_InitOuterCollider. */
    struct ContactCollider wideBodyCollider; /* 0x2D8: Ratatosk_InitWideBodyCollider. */
    u8 animation[0x20];                      /* 0x32C */
    u8 animationData[0x20];                  /* 0x34C */
    struct ContactPosition anchor;           /* 0x36C: collider and shadow position. */
    u8 unknown374[0x380 - 0x374];
    u8 spriteScaleX, spriteScaleY; /* 0x380: shrink and fade-out scale. */
    u8 unknown382[0x394 - 0x382];
    void *palette; /* 0x394: the active palette bank. */
    u8 unknown398[0x3AC - 0x398];
    struct RatatoskMotion motionPath;
    struct CameraTracking camera; /* 0x3DC */
    u8 shadow[0x40];              /* 0x40C: ScaledShadow. */
    u8 unknown44C[0x478 - 0x44C];
    u8 paletteAnimation[0x494 - 0x478]; /* 0x478: AnimData_SelectPaletteBank. */
    u32 displayResource;
    u8 unknown498[4];
    s32 animationIndex; /* 0x49C */
    u8 unknown4A0[0x4AC - 0x4A0];
    s8 blendStep; /* 0x4AC: palette blend position, 0 to 1 << blendShift. */
    u8 blendReturning;
    u8 blendMode; /* 0x4AE: 1 fade in, 2 fade out, 3 and 4 pulse. */
    u8 blendShift;
    u32 blendBank; /* 0x4B0 */
    u8 unknown4B4[4];
    u8 paletteBase[0x200]; /* 0x4B8 */
    u8 paletteNormal[0x200];
    u8 paletteAlternate[0x200];
    u8 paletteAlternate2[0x200]; /* 0xAB8 */
    u8 unknownCB8[4];
    u32 unknownCBC, unknownCC0; /* cleared every frame by the collider update. */
    u8 unknownCC4[4];
    s32 cloneTimer;                   /* 0xCC8 */
    struct ContactPosition points[9]; /* 0xCCC: path points from script keyword 'm'. */
    u8 unknownD14[4];
    struct RatatoskSlot orbPoints[9]; /* 0xD18 */
    struct RatatoskSlot targets[3];   /* 0xDF0 */
    u8 unknownE38[0xE4C - 0xE38];
    s16 selectedPoint, selectedTarget; /* 0xE4C */
    s16 distance;
    s16 scaleX, scaleZ; /* 0xE52: sprite scale, cleared by the directional animation update. */
    s16 scaleSpeed;
    s16 scaleMode;                      /* 0xE58: 0 normal, 1 stretched (960 by 1440). */
    u16 repeat;                         /* 0xE5A */
    struct ContactPosition destination; /* 0xE5C */
    struct ContactPosition center;      /* 0xE64 */
    u16 duration;                       /* 0xE6C */
    s16 frameDamage;                    /* 0xE6E: cleared every frame by the collider update. */
    u8 motionFlag;
    u8 unknownE71;
    u8 targetAngle; /* 0xE72 */
    u8 phaseTimer;
    s16 hitTimer;     /* 0xE74 */
    s16 terrainTimer; /* 0xE76 */
    s16 damageWindow; /* 0xE78 */
    s16 repeatHits;
    s16 flashTimer;    /* 0xE7C */
    s16 transferDelay; /* 0xE7E: sun-orb player transfer countdown. */
    s16 interruptLock;
    s16 transferArmed;
    s16 timer;    /* 0xE84 */
    s16 orbTimer; /* 0xE86 */
    s16 hasOrb;   /* 0xE88 */
    s16 previousAngle;
    s16 ended;
    s16 pattern;
    s16 choice;
    s16 previousOrb;
    u16 stateParameter; /* 0xE94 */
    s16 healthPhase;    /* 0xE96 */
    u32 lastAttack;
    s16 bodyAttack[3];       /* 0xE9C: attack, damage and kind of the body and outer colliders. */
    s16 frontAttack[3];      /* 0xEA2 */
    s16 attackEA8[3];        /* 0xEA8 */
    s16 wideAttack[3];       /* 0xEAE */
    s16 projectileAttack[3]; /* 0xEB4: homing projectile primary collider. */
    s16 secondaryAttack[3];  /* 0xEBA */
    s16 alternateAttack[3];  /* 0xEC0 */
    s16 cloneAttack[6];      /* 0xEC6 */
    u16 parameterED2;
    u16 invulnerability; /* 0xED4 */
    s16 damageTimer;     /* 0xED6: shows the hurt palette while positive. */
    u16 fade;            /* 0xED8 */
    s16 projectileCount;
    s16 cloneCount;
    s16 waitTimer; /* 0xEDE */
    s16 particleTimer;
    s16 orbReady;      /* 0xEE2 */
    s16 paletteToggle; /* 0xEE4 */
    s16 angleLimit;
    u32 savedPhase, savedTime, savedState, savedSequence; /* 0xEE8 */
    s16 pendingAttack;                                    /* 0xEF8 */
    s16 gate;
    u16 range[3]; /* 0xEFC */
    u8 unknownF02[2];
    s32 interval; /* 0xF04 */
    u32 incomingDamage;
    s16 contactValue; /* 0xF0C */
    s16 orbParameter;
    struct ContactPosition shotTarget; /* 0xF10: point tracked by the homing shots. */
    void *homingProjectiles;           /* 0xF18 */
    void *playerClones;
    void *particles;
    void *animatedParticles;
    void *sceneProjectiles;
    void *sunOrbs; /* 0xF2C */
};

typedef char RatatoskSizeCheck[sizeof(struct Ratatosk) == 0xF30 ? 1 : -1];

/* The StateActor recovery-pipeline operations (include/state_actor_action_entry.h) on
 * this type. */
static inline u8 Ratatosk_ConsumeActionEntry(struct Ratatosk *actor)
{
    if (actor->actionEnter) {
        actor->actionEnter = 0;
        actor->actionFinished = 0;
        return TRUE;
    }
    return FALSE;
}

/* SceneActors_SetFacing on this type: the body direction and the four- and eight-way
 * facings derived from it. */
static inline void Ratatosk_SetFacing(struct Ratatosk *actor, int angle)
{
    struct Ratatosk *self = actor;
    s32 shifted;
    u32 direction;
    self->body.direction = angle;
    shifted = (self->body.direction + 0x60) >> 6;
    direction = 3;
    direction &= shifted;
    self->facing = direction;
    shifted = (self->body.direction + 0x70) >> 5;
    direction = 7;
    direction &= shifted;
    self->facing8 = direction;
}

static inline void Ratatosk_SetAnimation(struct Ratatosk *actor, u32 animation)
{
    actor->actionAnimation = animation;
}

static inline u32 Ratatosk_TestFlags(const struct Ratatosk *actor, u32 mask)
{
    return actor->flags & mask;
}

static inline void Ratatosk_SetFlags(struct Ratatosk *actor, u32 flags)
{
    actor->flags |= flags;
}

static inline void Ratatosk_EnterPhase(struct Ratatosk *actor, u32 phase)
{
    actor->actionPhase = phase;
    actor->actionTimer = 0;
    actor->actionEnter = 1;
}

static inline void Ratatosk_StartAction(struct Ratatosk *actor, u32 mode, u32 index, u32 phase)
{
    actor->actionMode = mode;
    actor->actionIndex = index;
    Ratatosk_EnterPhase(actor, phase);
}

#endif
