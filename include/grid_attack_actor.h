#ifndef GUARD_GRID_ATTACK_ACTOR_H

#define GUARD_GRID_ATTACK_ACTOR_H

#include "engine.h"

#include "scene_vec.h"
#include "camera_tracking.h"
#include "contact_swarm.h"
#include "contact_actor.h"
#include "scene_event_dispatch.h"

/* Grid attack actor (tentative), allocation size 0x924. Unknown spans are
 * retained; behavioral field names are provisional. */

struct GridAttackPacked {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

struct GridAttackPacket {
    u16 id;
    u8 kind, zero;
    u16 reserved;
    u8 command, count;
    u16 args[1];
};

struct GridAttackTransform {
    u8 unknown0[9];
    u8 angle;
    u8 unknownA[2];
    struct ContactPosition coordinates;
};

struct GridAttackAnimData {
    u8 unknown0[0xC];
    void *palette;
    u8 unknown10[0xC];
};

struct GridAttackActor {
    u8 unknown0[0x48];
    struct GridAttackTransform transform; /* 0x48 */

    u8 unknown5C[0x34];
    u32 timer;     /* 0x90 */
    s16 health;    /* 0x94 */
    s16 maxHealth; /* 0x96 */
    u8 unknown98[0x2];
    u8 healthCheck; /* 0x9A */
    u8 unknown9B[0x1];
    u32 flags; /* 0x9C */
    u8 unknownA0[0x1];
    u8 controllerReset; /* 0xA1 */
    u8 stateReset;      /* 0xA2 */
    u8 unknownA3[0x1];
    u32 pending;       /* 0xA4 */
    u8 facing;         /* 0xA8 */
    u8 facing8;        /* 0xA9 */
    u8 requested;      /* 0xAA */
    u8 state;          /* 0xAB */
    u8 active;         /* 0xAC */
    u8 actionReset;    /* 0xAD */
    u8 action;         /* 0xAE */
    u8 finished;       /* 0xAF */
    u8 entered;        /* 0xB0 */
    u8 changed;        /* 0xB1 */
    u8 animationEnded; /* 0xB2 */
    u8 locked;         /* 0xB3 */
    u8 unknownB4[0x2];
    u8 animationMode;  /* 0xB6 */
    u8 animationSpeed; /* 0xB7 */
    u8 unknownB8[4];
    s32 contactSource;      /* 0xBC */
    u32 contactFlags;       /* 0xC0 */
    u32 actionTimer;        /* 0xC4 */
    u32 targetDistances[1]; /* 0xC8 */
    u8 requestedAngles[1];  /* 0xCC */
    u8 unknownCD[0x3];
    struct ContactPosition savedPosition; /* 0xD0 */
    u8 unknownD8[0x9];
    u8 attackRange;            /* 0xE1 */
    u8 contactRewardParameter; /* 0xE2 */
    u8 unknownE3;
    u32 effectTimer; /* 0xE4 */
    u8 unknownE8[8];
    u32 scriptArgs[4];                   /* 0xF0 */
    void (*callback)(void *);            /* 0x100 */
    u8 (*tryUpdate)(void *);             /* 0x104 */
    u8 (*reset)(void *);                 /* 0x108 */
    struct Player *players[1];           /* 0x10C */
    struct SceneVec *targetPositions[1]; /* 0x110 */
    s8 savedAngle;                       /* 0x114 */
    u8 unknown115[0x3];
    struct SceneEventReceiver receiver; /* 0x118 */
    u8 unknown140[0x34];
    u32 targetCount; /* 0x174 */
    s32 damage;      /* 0x178 */
    u8 special;      /* 0x17C */
    u8 unknown17D[0x1];
    u8 damageAngle; /* 0x17E */
    u8 stage;       /* 0x17F */
    u32 counter;    /* 0x180 */
    s16 height;     /* 0x184 */
    u8 unknown186[6];
    s16 flash;          /* 0x18C */
    u16 stun;           /* 0x18E */
    u16 actionDuration; /* 0x190 */
    u16 bodyMode;       /* 0x192 */
    u8 shakeAngle;      /* 0x194 */
    u8 shake;           /* 0x195 */
    u8 unknown196[0x1];
    u8 reactionCooldown;       /* 0x197 */
    s32 accumulated;           /* 0x198 */
    s32 threshold;             /* 0x19C */
    s16 attackCount;           /* 0x1A0 */
    u8 forceAction;            /* 0x1A2 */
    u8 attackAnimationHistory; /* 0x1A3 */
    s16 attackLimit;           /* 0x1A4 */
    u8 contactCountLimit;      /* 0x1A6 */
    u8 messageParameter;       /* 0x1A7 */
    u8 linkedActorPhase;       /* 0x1A8 */
    u8 unknown1A9[0x1];
    u16 phaseTimer;            /* 0x1AA */
    s16 attackPower;           /* 0x1AC */
    s16 attackKind;            /* 0x1AE */
    s16 attackParameter;       /* 0x1B0 */
    s16 projectileDamage;      /* 0x1B2 */
    u16 targetAttackParameter; /* 0x1B4 */
    u8 nearAttackDelay;        /* 0x1B6 */
    u8 farAttackDelay;         /* 0x1B7 */
    s16 contactReward;         /* 0x1B8 */
    s8 effectVariant;          /* 0x1BA */
    u8 effectDirection;
    u8 effectMode;     /* 0x1BC */
    u8 effectDuration; /* 0x1BD */
    u8 unknown1BE[0x2];
    u32 effectId;      /* 0x1C0 */
    u8 palette[0x200]; /* 0x1C4 */

    u8 normalDamageGraphics[0x200];      /* 0x3C4 */
    u8 specialDamageGraphics[0x200];     /* 0x5C4 */
    struct ContactCollider bodyCollider; /* 0x7C4 */
    struct ContactCollider damageCollider;
    struct GridAttackAnimData animData; /* 0x86C */

    struct ContactSwarmSprite drawable;    /* 0x888 */
    struct DisplaySequenceState animation; /* 0x8B4 */
    void *resource;                        /* 0x8C4 */
    struct ContactPosition shadowPosition; /* 0x8C8 */
    struct CameraTracking camera;
    void *projectedDisplay;         /* 0x900 */
    void *contactPool;              /* 0x904 */
    void *orbitContact;             /* 0x908 */
    void *contactBurst;             /* 0x90C */
    s32 linkedActorId;              /* 0x910 */
    struct GridAttackPacket packet; /* 0x914 */
    u8 unknown920[0x4];
};

void GridAttackActor_RemoveSprite(struct GridAttackActor *p);

void GridAttackActor_SetAnimation(struct GridAttackActor *p, int sequence, int start, int speed);

void GridAttackActor_OnBodyCollision(void);

void GridAttackActor_InitBodyCollider(struct GridAttackActor *p);

void GridAttackActor_InitDamageCollider(struct GridAttackActor *p, int mode);

void GridAttackActor_GetGridPosition(s16 *p, int x, int z);

void GridAttackActor_SpawnGridContact(struct GridAttackActor *p, int x, int z);

void GridAttackActor_SpawnGridPattern(struct GridAttackActor *p);

int GridAttackActor_OnDamage(struct GridAttackActor *p, int kind, int damage);

void GridAttackActor_ResetAttackAnimationHistory(struct GridAttackActor *p);

int GridAttackActor_SelectAttackAnimation(struct GridAttackActor *p);

void GridAttackActor_EmitSpriteParticles(struct GridAttackActor *p);

void GridAttackActor_RequestLinkedActorState(struct GridAttackActor *p);

void GridAttackActor_NotifyLinkedActorLatePhase(struct GridAttackActor *p);

void GridAttackActor_UpdateIdle(struct GridAttackActor *p);

void GridAttackActor_UpdateGridAttack(struct GridAttackActor *p, int tick);

void GridAttackActor_UpdateElevatedGridAttack(struct GridAttackActor *p);

void GridAttackActor_UpdateTargetedAttack(struct GridAttackActor *p, int tick);

void GridAttackActor_UpdateDamageHold(struct GridAttackActor *p);

void GridAttackActor_UpdateDamageRecovery(struct GridAttackActor *p, int tick);

void GridAttack_UpdateStaggerState(struct GridAttackActor *p);

void GridAttack_UpdateDeathState(struct GridAttackActor *p);

void GridAttack_UpdateHiddenState(struct GridAttackActor *p);

void GridAttack_UpdateIdleState(struct GridAttackActor *s);

void GridAttack_UpdateAnimationNineState(struct GridAttackActor *p);

void GridAttack_UpdateAnimationElevenState(struct GridAttackActor *s);

void GridAttack_UpdateAlternateIdleState(struct GridAttackActor *s);

void GridAttack_UpdateRandomAnimationState(struct GridAttackActor *p, int tick);

void GridAttack_UpdateAnimationThreeState(struct GridAttackActor *p);

void GridAttack_UpdateFastAnimationThreeState(struct GridAttackActor *p);

void GridAttack_UpdateTimedHideState(struct GridAttackActor *p, int tick);

void GridAttack_RunStateIfUnpaused(u32 a);

u32 GridAttack_ApplyRequestedState(struct GridAttackActor *p);

int GridAttack_ApplyPendingDamage(struct GridAttackActor *p);

int GridAttack_SelectNextAction(struct GridAttackActor *p);

void GridAttack_UpdateStateNineAction(struct GridAttackActor *p);

void GridAttack_UpdateIdleAction(struct GridAttackActor *p);

void GridAttack_UpdateStateElevenAction(struct GridAttackActor *p);

void GridAttack_UpdateStateTwelveAction(struct GridAttackActor *p);

void GridAttack_UpdateStateThirteenAction(struct GridAttackActor *p);

void GridAttack_UpdateAnimationFourteen(struct GridAttackActor *p);

void GridAttack_UpdateAnimationFifteen(struct GridAttackActor *p);

void GridAttack_UpdateAnimationSixteen(struct GridAttackActor *p);

void GridAttack_UpdateAnimationSeventeen(struct GridAttackActor *p);

void GridAttack_DispatchSpecialAnimation(struct GridAttackActor *p);

int GridAttack_TryStartDamageReaction(struct GridAttackActor *p);

void GridAttack_UpdateWaitAction(struct GridAttackActor *p);

void GridAttack_UpdateActionTwo(struct GridAttackActor *p);

void GridAttack_UpdateDeathAction(struct GridAttackActor *p);

void GridAttack_UpdateAttackAction(struct GridAttackActor *p);

void GridAttack_UpdateAnimationFive(struct GridAttackActor *p);

void GridAttack_UpdateRecoveryAction(struct GridAttackActor *p);

void GridAttack_UpdateDamageAction(struct GridAttackActor *p);

void GridAttack_UpdatePassiveAction(struct GridAttackActor *p);

void GridAttack_DispatchNormalAction(struct GridAttackActor *p);

void GridAttack_DispatchReactionAction(struct GridAttackActor *p);

void GridAttack_UpdateNormalActions(struct GridAttackActor *p);

void GridAttack_UpdateReactionActions(struct GridAttackActor *p);

void GridAttack_UpdateSpecialActions(struct GridAttackActor *p);

void GridAttack_UpdateActionController(struct GridAttackActor *p);

int GridAttack_Update(struct GridAttackActor *p);

u32 GridAttackActor_Destroy(struct GridAttackActor *p);

struct GridAttackActor *GridAttackActor_Create(u32 a);

#endif
