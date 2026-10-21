#ifndef GUARD_LARGE_SCENE_ANIMATION_CONTROLLER_H
#define GUARD_LARGE_SCENE_ANIMATION_CONTROLLER_H
#include "global.h"
#include "engine.h"
#include "scene_vec.h"
#include "scene_event_dispatch.h"

/* Tentative: Nidhoggr, a dragon boss task (allocated at 0x72FC bytes, Task_Create(8, ...)).
 * It has 26 body-segment "parts", each with its own animation row, collider and small
 * extra record, but only 9 of them track independent hit points (the rest are purely
 * cosmetic). `mode` selects one of its ~19 high-level behaviors (entrance, two attack
 * phases, retreat, cutscene reactions, ...) via the table at gUnk_0824F5C0, and `state`
 * is the sub-state within whichever behavior is active. Field names are provisional
 * (confidence=low); several short byte/word fields, and most of the suspended-object
 * and terrain-cell sub-regions, are still guesses. */

struct NidhoggrAnimRow {
    u8 unknown0[8];
    u32 flags;
    u8 unknown12[0x20 - 12];
    struct SceneVec pos;
    u8 unknown28[0x60 - 0x28];
};

struct NidhoggrCollider {
    u8 unknown[0x54];
};

struct NidhoggrPartExtra {
    u8 unknown[0x10];
};

struct NidhoggrTerrainCell {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x10 - 8];
};

struct Nidhoggr {
    struct Task task;                   /* 0x00 */
    struct SceneEventReceiver receiver; /* 0x18 */
    u8 unknown40[0x4C - 0x40];
    u32 actorId;                  /* 0x4C: id used for SceneMessages/MessageQueue routing */
    void *bgScript;               /* 0x50: ScriptProc_Start handle for the background layers */
    u8 effectPool[0x5404 - 0x54]; /* 0x54: small/large sprite-effect slot pools */
    struct SceneVec pos;          /* 0x5404: current world position */
    struct SceneVec basePos;      /* 0x540C: anchor/base position that sway offsets add to */
    s16 speed;                    /* 0x5414 */
    s16 bgScrollX;                /* 0x5416 */
    s16 bgScrollY;                /* 0x5418 */
    u8 unknown541A[0x541C - 0x541A];
    u8 animHdr[0x20];                /* 0x541C: shared keyframe/resource header for all part rows */
    struct NidhoggrAnimRow rows[26]; /* 0x543C: one per body segment */
    struct NidhoggrCollider
        altColliders[26]; /* 0x5DFC: a second collider per part; [0] is refreshed
                                                * each frame in Update while flags[0] counts down */
    u8 unknown6684[0x66D8 - 0x6684];
    struct NidhoggrCollider colliders[26];  /* 0x66D8 */
    struct NidhoggrPartExtra partExtra[26]; /* 0x6F60 */
    struct NidhoggrTerrainCell
        terrainCells[2];      /* 0x7100: registered via TerrainModifiers_RegisterCell */
    u8 suspendedAnim[0x1C];   /* 0x7120: suspended-object sprite animation data */
    u8 suspendedSprite[0x1C]; /* 0x713C */
    s16 suspendedPosX, suspendedPosY, suspendedPosZ, suspendedPosW; /* 0x7158 */
    u8 unknown7160[0x7168 - 0x7160];
    u8 suspendedCollider[0x54]; /* 0x7168 */
    u8 suspendedState;          /* 0x71BC: descent/retraction/shake sub-state */
    u8 terrainCellsRegistered;  /* 0x71BD */
    u8 attackActive;            /* 0x71BE: timed-attack collider active latch */
    u8 unknown71BF;
    u16 suspendedTimer; /* 0x71C0 */
    u8 unknown71C2[0x71DC - 0x71C2];
    u8 rowInitFlag[26]; /* 0x71DC: per-row "keyframes loaded" latch, indexed like rows[] */
    u8 rowCounter[26];  /* 0x71F6: per-row counter bumped by Nidhoggr_UpdatePartMotion */
    u8 rowDoneFlag[26]; /* 0x7210: per-row "animation finished" latch */
    u8 rowBlink[26];    /* 0x722A: per-row blink countdown */
    s16 timer;          /* 0x7244: generic frame counter for the current state */
    u16 timer2;         /* 0x7246: a second, apparently independent frame counter
                                          * used the same way by a different set of behaviors */
    u8 active;          /* 0x7248 */
    u8 mode;   /* 0x7249: top-level behavior select (Nidhoggr_SetBehavior); drives gUnk_0824F5C0 */
    u8 state;  /* 0x724A: sub-state within the current behavior */
    u8 bgMode; /* 0x724B: Nidhoggr_SetBackgroundMode's current mode */
    u8 flags[9];        /* 0x724C: per-part misc flags (meaning mostly unconfirmed) */
    u8 dead[9];         /* 0x7255: per-part defeated latch */
    u8 shuffleCursor;   /* 0x725E: Nidhoggr_GetNextShuffledPart index */
    u8 shuffleOrder[9]; /* 0x725F */
    u8 unknown7268[0x7268 - 0x7268];
    u16 maxLife;     /* 0x7268: option -l, default 4000 */
    u16 maxHp;       /* 0x726A: option -g, default 100; per-part max hp */
    u16 optionY;     /* 0x726C: option -Y, default 1000 */
    u16 attackPower; /* 0x726E: option -A, default 40; suspended object's collider power */
    u16 optionD;     /* 0x7270: option -D, default 20 */
    u16 optionE;     /* 0x7272: option -E, default 100 */
    u16 optionS;     /* 0x7274: option -S, default 0 */
    s16 life;        /* 0x7276: boss total life (option -l, kept twice) */
    u8 unknown7278[0x7278 - 0x7278];
    s16 hp[9];    /* 0x7278: per-part current hp */
    s16 groupHpA; /* 0x728A */
    s16 groupHpB; /* 0x728C */
    u8 unknown728E[0x7290 - 0x728E];
    struct SceneVec
        cameraPos; /* 0x7290: current camera position (ApproachCameraTarget eases toward cameraTarget) */
    struct SceneVec cameraTarget; /* 0x7298: configured camera target */
    u16 targetSourceX, targetSourceY,
        targetSourceZ; /* 0x72A0: option -c (two) or -f (three) values */
    u8 targetMode;     /* 0x72A6: 0 if configured via -c, 1 via -f */
    u8 unknown72A7;
    s32 posOffsetX;         /* 0x72A8: coarse horizontal motion, stepped by posOffsetXVelocity */
    s32 swayX;              /* 0x72AC: small bounded horizontal sway, stepped by swayXVelocity */
    s32 posOffsetZ;         /* 0x72B0: coarse depth motion, stepped by posOffsetZVelocity */
    s32 posOffsetXVelocity; /* 0x72B4 */
    s32 swayXVelocity;      /* 0x72B8 */
    s32 posOffsetZVelocity; /* 0x72BC */
    u32 script;             /* 0x72C0: option -e; a script key to call when a cutscene departs */
    u8 unknown72C4[0x72FC - 0x72C4];
};

/* Defined outside this unit's range; callers disagree on its exact signature. */
s32 Nidhoggr_ChangeCompletedPartState(void *, s32, s32);

/* Declared once here so every caller in this unit (which may come before or after the
 * definition in ROM order) agrees on the signature. */
void Nidhoggr_ActivateRandomPart(struct Nidhoggr *);
void Nidhoggr_ApproachCameraTarget(struct Nidhoggr *);
void Nidhoggr_CheckActivationTriggers(struct Nidhoggr *);
void Nidhoggr_ClearTargetValues(struct Nidhoggr *);
s32 Nidhoggr_Destroy(struct Nidhoggr *);
s32 Nidhoggr_DestroyCutscene(struct Nidhoggr *);
void Nidhoggr_DestroyParts(struct Nidhoggr *);
void Nidhoggr_DestroySuspendedObjectSprite(struct Nidhoggr *);
s32 Nidhoggr_GetNextShuffledPart(struct Nidhoggr *);
s32 Nidhoggr_Init(struct Nidhoggr *, s32, s32);
void Nidhoggr_InitBackgrounds(struct Nidhoggr *);
void Nidhoggr_InitBobbingMotion(struct Nidhoggr *, s32, s32);
void Nidhoggr_InitFirstPhaseParts(struct Nidhoggr *);
void Nidhoggr_InitRelativeMotion(struct Nidhoggr *, s32, s32);
void Nidhoggr_InitSuspendedObject(struct Nidhoggr *);
u32 Nidhoggr_IsSuspendedObjectHidden(struct Nidhoggr *);
u32 Nidhoggr_IsSuspendedObjectReady(struct Nidhoggr *);
void Nidhoggr_ReadOptions(struct Nidhoggr *);
void Nidhoggr_RegisterTerrainCells(struct Nidhoggr *);
void Nidhoggr_ResetAllPartHealth(struct Nidhoggr *);
void Nidhoggr_SetBackgroundMode(struct Nidhoggr *, u32);
void Nidhoggr_SetBehavior(struct Nidhoggr *, s32);
void Nidhoggr_SetConfiguredTargetValues(struct Nidhoggr *);
void Nidhoggr_SetTerrainCellFlag40(struct Nidhoggr *);
void Nidhoggr_ShufflePartOrder(struct Nidhoggr *);
void Nidhoggr_StartSuspendedObjectDescent(struct Nidhoggr *);
void Nidhoggr_StartSuspendedObjectRetraction(struct Nidhoggr *);
void Nidhoggr_StartSuspendedObjectShake(struct Nidhoggr *);
void Nidhoggr_TrackPlayerDepth(struct Nidhoggr *);
void Nidhoggr_UnregisterTerrainCells(struct Nidhoggr *);
s32 Nidhoggr_Update(struct Nidhoggr *);
void Nidhoggr_UpdateAnchoredSway(struct Nidhoggr *, s32, s32);
void Nidhoggr_UpdateBackgroundScroll(struct Nidhoggr *);
s32 Nidhoggr_UpdateCutscene(struct Nidhoggr *);
void Nidhoggr_UpdateHorizontalSway(struct Nidhoggr *);
void Nidhoggr_UpdateState(struct Nidhoggr *);
void Nidhoggr_UpdateWideSway(struct Nidhoggr *);
void Nidhoggr_ActivateRandomParts2(struct Nidhoggr *);

#endif
