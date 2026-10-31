#ifndef GUARD_PLAYER_H
#define GUARD_PLAYER_H

#include "global.h"
#include "scene_vec.h"

/*
 * The player actor. gPlayers holds four pointers (one per player slot; slot 0 is the local
 * player, the other slots are used by the multiplayer code, guess). Built from the offsets seen
 * across src/ (partial views in src/fn/*.c); unknown ranges are u8 unkXXX[N]. Names are
 * provisional.
 */
struct PlayerActionRequest {
    s8 kind;
    u8 enabled, latched, duration, blocked;
};

struct PlayerIndexedAction {
    u8 unknown0, duration, unknown2, unknown3;
};

struct PlayerActionDescriptor {
    u8 kind, displaySelection, unknown2, duration;
    u32 unknown4[7];
};

struct PlayerActionConfig {
    u8 kind, unknown1, unknown2, duration;
    u8 unknown4[0x10];
    u16 catalogOption;
};

struct PlayerActionEntry {
    u8 kind, unknown1;
    u16 remainingCharge;
};
struct PlayerActionDrainContext {
    u8 unknown0[0x3E];
    u16 pendingCost;
};
struct PlayerStatModifierContext {
    u8 unknown0[0x48];
    struct PlayerActionDrainContext drain;
    u8 unknown88[2];
    u8 contactDirection; /* owner + 0xBCE */
    u8 unknown8B[0x19];
    struct SceneVec contactOrigin;         /* owner + 0xBE8 */
    struct PlayerActionEntry *actionEntry; /* owner + 0xBF0: descriptor source */
    u8 actionKind;                         /* owner + 0xBF4 */
    u8 statBOverrideEnabled;               /* owner + 0xBF5 */
    u8 unknownB2[2];
    u16 actionDuration, actionPhase, actionTimer; /* owner + 0xBF8 */
    u16 contactParameter;                         /* owner + 0xBFE */
    u16 contactExtentA;                           /* owner + 0xC00 */
    u8 unknownBE[2];
    u16 contactExtentB; /* owner + 0xC04 */
    u8 unknownC2[2];
    u16 contactCounter;  /* owner + 0xC08 */
    u16 contactTimer;    /* owner + 0xC0A */
    u8 contactHeading;   /* owner + 0xC0C */
    s8 savedActionIndex; /* owner + 0xC0D: snapshot from Player_GetActionIndex (tentative) */
    u8 unknownCA[0xA];
    u8 statBOverrideMode;  /* owner + 0xC18: seven selects GameState stat B */
    u8 thresholdSatisfied; /* owner + 0xC19, tentative */
    u8 unknownD6[2];
    u8 actionSelectionDisplay[0x50]; /* owner +0xC1C: configured by 0814DE0C */
    u8 unknown128[8];
    s32 attachedEntry;              /* owner + 0xC74: negative means absent */
    void *statModifierCallbacks[3]; /* owner + 0xC78: applied in order */
};

struct PlayerColliderVector {
    u32 x : 16, y : 16, z : 16;
};

struct PlayerCollider {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x38];
};

struct PlayerCueCollider {
    u8 unknown0[0x34];
    u32 primaryMask, flags;  /* 0x34, 0x38: contact categories */
    u16 power, appliedPower; /* 0x3C, 0x3E */
    u8 unknown40[2];
    u8 direction; /* 0x42: quantized facing angle */
    u8 reaction;  /* 0x43: primary, secondary or default category */
    u8 unknown44[0x2C];
};

struct PlayerAnimation {
    u8 unknown0[8];
    u32 flags;
};

struct PlayerVisualObject {
    u8 unknown0[6];
    u16 resource;
    u8 unknown8[4];
    u32 parameters;
};

struct Player {
    u8 unk0[0x18];
    s32 slot;                  /* 0x18: compared with local link player index */
    u32 kind;                  /* 0x1C: kind one accepts mode-0x1A position reset */
    u32 statusFlags;           /* 0x186c: busy bits, 0x100000: cannot interact (guess) */
    u16 positionalEffectGroup; /* 0x24: forwarded to SceneStimulus_AddTagged (tentative) */
    u8 unk26[6];
    u8 movementRequestBlocked; /* 0x2C: gates saved movement target; also indexes player slot on teardown */
    s8 unk2D;                  /* forwarded to indexed scene-entry update */
    u8 unk2E[2];
    struct SceneVec position;
    s16 velocityX; /* 0x38: direction-derived velocity */
    u16 unk3A;
    s16 velocityZ;
    u8 unk3E[0x68 - 0x3E];
    u16 indicatorStep; /* 0x68: cycles 0-11, driven by indicatorUpdateRate; >>2 feeds position.x (tentative) */
    u8 unk6A[2];
    u8 animationData[0x20];           /* 0x6C: keyframe resource */
    struct PlayerAnimation animation; /* 0x8C */
    u16 actionCueCounter;             /* 0x98: zero gates phase-one sound in 08146478 (tentative) */
    u8 unk9A[6];
    u16 actionCueFlags; /* 0xA0: low two bits gate phase-one sound (tentative) */
    u8 unkA2[0xA];
    struct SceneVec previousPosition; /* 0xAC: synchronized after terrain clamp */
    u8 unkB4[0xC];
    u8 movementSpeedScale;  /* 0xC0: mode-five speed scale, units of 1/64 */
    u8 secondarySpeedScale; /* 0xC1: paired with movementSpeedScale by 08145400 (tentative) */
    u8 unkC2[4];
    u16 spriteResource; /* 0xC6 */
    u8 unkC8[0xC];
    u32 actionVariantResource; /* 0xD4: forwarded to variant display */
    u8 unkD8[0x14];
    struct PlayerAnimation secondaryAnimation; /* 0xEC: alternate keyframe channel */
    u8 unkF8[0x14];
    u16 alternatePosition[3]; /* 0x10C */
    u8 unk112[0x3A];
    u32 secondaryDrawFlags; /* 0x14C: bit zero follows reset flag one */
    u8 unk150[0x18];
    struct SceneVec colliderPosition; /* 0x168 */
    u8 unk170[8];
    struct PlayerVisualObject *visualObject; /* 0x178 */
    u8 unk17C[0x54];
    u8 auxiliarySprite[0x40]; /* 0x1D0: removed by player destructor */
    u8 unk210[0x20];
    struct PlayerCollider collider; /* 0x230: embedded collider, extent bounded by directionFrame */
    u16 directionFrame;             /* 0x270: reset by forced direction setup */
    u8 savedHeading;                /* 0x272: snapshot of GameState heading byte (tentative) */
    u8 unk273;
    u16 positionResetValue; /* 0x274 */
    u8 resetFlags;          /* 0x276: bit one cleared by player reset */
    u8 unk277[0x11];
    u16 storedTerrainCell;   /* 0x288 */
    u16 recoveryTerrainCell; /* 0x28A: terrain cell used for recovery facing */
    u8 unk28C[3];
    u8 velocityScaleAxis; /* 0x28F: asymmetric X/Z velocity scaling */
    u8 unk290[4];
    u8 positionTracker[4];            /* 0x294: refreshed after player update */
    struct SceneVec *trackedPosition; /* 0x298 */
    u8 unk29C[0x4C];
    u8 contactListNode
        [0x10]; /* 0x2E8: a struct SceneActorContactNode (scene_actor_contacts.h; not included here, it pulls in engine.h) */
    u8 unk2F8[0x34];
    u8 actionFlagState[4];     /* 0x32C: refreshed after action modifiers */
    u16 movementLoadPercent;   /* 0x330: reduces base movement speed (tentative) */
    u16 menuSnapshotValues[3]; /* 0x332: copied to menu panel +0x48B0 (tentative) */
    u8 unk338[4];
    u8 thresholdMode; /* 0x33C: values 6 through 11 qualify for the scene threshold */
    u8 actionEnabled; /* 0x33D: zero bypasses action mode filtering */
    u8 actionSelectionStamp,
        actionModeValue;     /* 0x33E: cached selection and mode value (tentative) */
    u8 actionRequestBlocked; /* 0x340 */
    u8 unk341[3];
    const struct PlayerActionConfig *actionConfig; /* 0x344 */
    const u16 *inputKeys;     /* 0x348: directional key bits consumed by Player_GetInputDirection */
    u16 directionHistory[10]; /* 0x34C: signed samples; 0xFFFF means absent (read through s16) */
    u8 visualParameters[0xA]; /* 0x360: animation parameter block */
    u16 visualParameterA, visualParameterB; /* 0x36A */
    u8 unk36E[0xC];
    u16 visualParameterC; /* 0x37A */
    u8 unk37C[0x24];
    u16 animationIndex; /* 0x3A0: selected base keyframe index */
    u8 heading, side;   /* 0x3A2: folded direction and side */
    u8 facing;          /* (guess) direction index */
    u8 unk3A5[2];
    u8 cueColliderPhase;                  /* 0x3A7: collider setup phase (tentative) */
    struct PlayerCueCollider cueCollider; /* 0x3A8 */
    u8 actionMode; /* 6: alternate effect mode / locked containers refuse to open (guess) */
    u8 unk419;
    u16 resetValue41A;
    u16 derivedStats[3]; /* 0x41C: capped at 120 by 0813B4C8 */
    u8 unk422[2];
    u16 currentStatA, maximumStatA;             /* 0x424: tentative health-like stat */
    u16 currentStatB, maximumStatB;             /* 0x428: tentative energy-like stat */
    u16 statHundredths, statWhole, statusValue; /* 0x42C */
    u8 unk432[2];
    u16 statScale; /* 0x434: initialized to 64 by 0813B67C */
    u8 unk436[0xC];
    u16 particleEmissionCount; /* 0x442 */
    u32 statBonusFlags;   /* 0x444: bit 27 enables proportional currentStatA bonus (tentative) */
    u32 actionFlags;      /* bit 4: items always visible (guess) */
    u32 actionStateFlags; /* 0x44C: bit 4 marks mode three (tentative) */
    u8 cueDefenses[6];    /* 0x450: six contact-category reductions (tentative) */
    u8 unk456;
    u8 tetherMode;  /* 0x457: mode six selects forced cue parameter */
    u8 tetherPhase; /* 0x458: mode-nine palette/callback gate (tentative) */
    u8 unk459;
    u16 tetherFrame; /* 0x45A: reset on mode/phase change (tentative) */
    u8 unk45C;
    u8 statGaugeFull;           /* 0x45D: value 1 forces ten filled gauge cells (081647A4). */
    u8 movementStatTimer;       /* 0x45E: cycles through 0..60 */
    u8 historyDirectionMode;    /* 0x45F: 1 selects direction from history */
    u8 actionRequestRestricted; /* 0x460: rejects action requests one and two */
    u8 unk461[3];
    u8 actionLatch464, actionLatch465;
    u8 unk466[0xC];
    u8 timedEventSlot; /* 0x472: index into GameState counters620/counters628 */
    u8 unk473;
    u16 tetherStatusFlags,
        tetherStatusTag; /* 0x474: compared with TetherActor statusFlags/statusTag */
    u8 terrainClampApplied, terrainClampBlocked; /* 0x478 */
    u8 terrainRecoveryPhase, terrainRecoveryActive;
    struct SceneVec *terrainTarget;    /* 0x47C */
    struct SceneVec terrainCellCenter; /* 0x480 */
    u8 actionPhase488, actionActive489;
    u8 unk48A[2];
    u8 movementVelocityLocked; /* 0x48C: suppresses velocity recalculation */
    u8 unk48D[0x17];
    u8 actionFourEnabled; /* 0x4A4: permits action 4 in modes 0, 1 and 6 (tentative) */
    u8 coordinateAxis;    /* 0x4A5: 1/5 select X, other values select Z (tentative) */
    u8 unk4A6[2];
    struct SceneVec cameraTarget;                     /* 0x4A8: blended focus position */
    s16 cameraOffsetX, cameraOffsetY, cameraOffsetZ;  /* 0x4B0 */
    s16 cameraOffsetActive;                           /* 0x4B6 */
    u8 cameraOffsetUpdateState, cameraInputProcessed; /* 0x4B8: tentative */
    u8 contactSpecial;                                /* 0x4BA: special collision reaction */
    u8 unk4BB;
    u16 interactionResetCounter; /* 0x4BC: cleared before interaction mode 0x17 */
    u8 positionResetState;       /* 0x4BE */
    u8 unk4BF[3];
    u16 contactActionTag; /* 0x4C2: terrain recovery sets 0x191 */
    u8 unk4C4[4];
    u16 actionAccumulator;       /* 0x4C8: wraps by 100 and triggers Player_EmitPeriodicEffect */
    u16 particleIntervalCounter; /* 0x4CA: periodic particle emission */
    u8 unk4CC[4];
    u16 particleCadenceCounter;                   /* 0x4D0: emits a particle after fifteen ticks */
    u16 contactMagnitude, contactDirection;       /* 0x4D2: deferred collision response */
    u16 positionResetRequested;                   /* 0x4D6: request flag, tentative */
    u16 positionResetTimer, positionResetCounter; /* 0x4D8 */
    u8 unk4DC[4];
    u8 cueVariant; /* 0x4E0: copied by 0815B404, tentative */
    u8 unk4E1[7];
    u16 terrainRecoveryCounter; /* 0x4E8 */
    u8 unk4EA[2];
    u16 actionCountdown;      /* 0x4EC: mode phase subtracts thirty per update */
    u8 actionEighteenEnabled; /* 0x4EE: selects phase one and refreshes action parameters */
    u8 unk4EF[3];
    u8 movementRequestAnimated; /* 0x4F2: inverse of immediate movement flag */
    u8 movementAnimationState;  /* 0x4F3: negative byte resets animation (tentative) */
    u8 facingChangeEffects;     /* 0x4F4: bits zero/one enable/select facing cue */
    u8 savedEffectTick;         /* 0x4F5 */
    u8 movementSpeedReduction;  /* 0x4F6 */
    u8 unk4F7;
    u16 chargeCounter; /* 0x4F8 (tentative) */
    u8 unk4FA[3];
    u8 chargeSoundActive; /* 0x4FD (tentative) */
    u8 unk4FE[0x22];
    u16 interactionPromptFlags; /* 0x520: bits OR'd in by ControlPanel based on interactionPromptCategory (tentative) */
    u16 displaySegmentCount; /* 0x522: tentative, clamped to seven by segment display */
    u16 indicatorUpdateRate; /* 0x524: divisor for 128/x step size of indicatorStep (tentative) */
    u8 unk526[8];
    u16 actionAccumulatorBlocked; /* 0x52E: nonzero prevents accumulation */
    u16 scriptStatus[2];          /* 0x530: script keyword s selects a nonzero status */
    u16 sceneMode;                /* (guess) */
    u16 unk536;
    u16 active;        /* (guess) */
    u8 contactCounted; /* 0x53A: threshold counter latch (tentative) */
    u8 contactThresholdPending,
        contactThresholdReady; /* 0x53B: two-stage threshold event latch (tentative) */
    u8 contactCueDuration;     /* 0x53D: threshold cue duration */
    u8 unk53E[0xC];
    u8 catalogSavedOption; /* 0x54A: menu option copied when scriptStatus[1] is zero (tentative) */
    u8 unk54B[0x2D];
    u16 interactionPromptCategory; /* 0x578: selects which bit ControlPanel ORs into interactionPromptFlags (tentative) */
    u8 unk57A[2];
    u16 indicatorTickCounter; /* 0x57C: wraps 0-9; gates a displaySegmentCount-pitched tick sound (tentative) */
    u8 unk57E[0xE];
    void (*triggerAnimationCallback)(struct Player *); /* 0x58C */
    u8 unk590[8];
    u16 recoilSpeed;             /* 0x598: decays by three quarters per update */
    u8 animationBranchRequested; /* 0x59A: selects phase one in 081502AC */
    u8 cuePhase;                 /* 0x59B: reset by cue-seven mode two (tentative) */
    u8 kindTwoEntered;           /* 0x59C: set by 0815AD78, tentative */
    s8 targetFacing;             /* 0x59D: shortest-turn target in 0814FA10 */
    u8 kindTwoState;             /* 0x59E: cleared by 0815AD78, tentative */
    u8 unk59F[4];
    u8 pendingAnimationTrigger; /* 0x5A3: consumed by 0814F90C */
    u32 cueParameter;           /* 0x5A4: forwarded cue parameter, tentative */
    u8 unk5A8[0x10];
    u8 effectGroup; /* 0x5B8: scene effect group context */
    u8 movementModifierKind, unknown5BA, movementModifierActive; /* 0x5B9: tentative */
    u8 unk5BC[0x134];
    u16 effectPosition[3]; /* 0x6F0 */
    u8 unk6F6[2];
    u16 resetValue6F8, resetValue6FA, resetValue6FC;
    u8 unk6FE[2];
    u8 shadow[0x40];  /* 0x700: actor shadow state */
    u8 resetState740; /* 0x740: set to one by 0813B080 */
    u8 unk741[7];
    u8 visualEffects748[0x30], visualEffects778[0x28];
    u8 visualEffects7A0[0x30], visualEffects7D0[0x2C];
    u8 directionalEffect[0x68];      /* 0x7FC */
    void *directionalEffectCallback; /* 0x864 */
    u8 unk868[0x218];
    struct SceneVec actionOrigin; /* 0xA80: offset copy of position (tentative) */
    u8 unkA88[4];
    struct SceneVec movementStart, movementDestination; /* 0xA8C, 0xA94 */
    u16 movementDuration;                               /* 0xA9C */
    u8 unkA9E[0x10];
    u16 actionResetCounter;  /* 0xAAE: reset when starting a variant action (tentative) */
    u16 menuStatusValues[7]; /* 0xAB0: panel selects indices 0, 2 or 6 (tentative) */
    u8 unkABE[4];
    u16 baseSpriteResource, savedSpriteResource; /* 0xAC2 */
    u8 unkAC6[2];
    u8 baseActionVariant; /* 0xAC8: copied to actionVariant */
    u8 actionVariant;     /* 0xAC9: zero or saved action index plus one (tentative) */
    u16 savedVisualMode;  /* 0xACA */
    s16 visualFlashTimer; /* 0xACC */
    u8 unkACE[4];
    u16 movementRequestParameter, movementRequestScale;  /* 0xAD2: tentative */
    u16 actionEighteenParameter, actionEighteenDuration; /* 0xAD6: tentative */
    u16 actionRequestHold, actionRequestTimer;           /* 0xADA */
    u8 unkADE[0xA];
    struct SceneVec movementRequestTarget; /* 0xAE8 */
    u8 unkAF0;
    u8 actionRequestState; /* 0xAF1 */
    u8 unkAF2[0x12];
    void *kindTwoAttachment;        /* 0xB04: descriptor-driven attachment, tentative */
    u8 kindTwoAttachmentDescriptor; /* 0xB08 */
    u8 unkB09;
    u16 kindTwoAttachmentCounter; /* 0xB0A */
    u8 unkB0C[0x30];
    s32 eventProcedure;                                   /* 0xB3C */
    void *actionRequestEffect;                            /* 0xB40 */
    struct PlayerStatModifierContext statModifierContext; /* 0xB44: callback context */
    void *actionDisplay;           /* 0xC84: receives descriptor display selection */
    void (*actionCallback)(void);  /* 0xC88 */
    void *flagCallbacks[3];        /* 0xC8C: callback results combined with OR (tentative) */
    void *contactCostCallbacks[3]; /* 0xC98: additional contact cost callbacks */
    void *updateCallback;          /* 0xCA4: called with the player each update */
};

/* 0x02000560 */
extern u32 gPlayerSlotStatus[4]; /* one status word per player, cleared with the player slots */
/* 0x02000580 */
extern struct Player *gPlayers[4];

#endif
