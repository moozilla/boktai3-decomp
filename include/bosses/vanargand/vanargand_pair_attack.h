#ifndef GUARD_VANARGAND_PAIR_ATTACK_H
#define GUARD_VANARGAND_PAIR_ATTACK_H

#include "bosses/vanargand/paired_scene_actor.h"
#include "engine/math/scene_vec.h"
#include "bosses/vanargand/positioned_effect_coordinates.h"
#include "bosses/vanargand/vanargand_damage_part.h"
#include "bosses/vanargand/vanargand_stage_colors.h"

struct Player;
struct VanargandBodyAnimation {
    u8 unknown0[0x18];
    u16 flags;
    u8 unknown1A[6];
    struct SceneVec position;
    u8 unknown28[0x60 - 0x28];
};

/* The body's current action sequence: Vanargand_StopActionSequence clears its step and
 * timer and stops its song. */
struct VanargandActionSequence {
    u8 unknown0[0x14];
    u8 playerReady; /* 0x14 (body +0x230): player action mode is 0 or 2 (tentative). */
    u8 step;
    u16 timer;
    u8 unknown18[0x1E0 - 0x18];
};

void Vanargand_StopActionSequence(struct VanargandActionSequence *sequence);

struct VanargandPairAttack {
    u8 pad0[0x48];
    u32 bodyHeader[3]; /* SceneBody header preceding the world position. */
    struct SceneVec position;
    u8 pad5C[0x90 - 0x5C];
    u32 callbackFrame; /* 0x90: StateActor callbackFrame. */
    s16 health;
    u8 pad96[6];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 action;
    u32 timer;
    u8 padA8[2];
    u8 requestedAnimation; /* 0xAA: StateActor actionAnimation. */
    u8 state;
    u8 actionMode, actionIndex, actionPhase; /* 0xAC */
    u8 finished;
    u8 entered;
    u8 pending;
    u8 animationEvent;
    u8 padB3[0xC4 - 0xB3];
    u32 actionTimer; /* 0xC4: frames in the current action phase. */
    u8 padC8[0x100 - 0xC8];
    void (*stateCallback)(void *, s32);
    u8 (*callback)(struct VanargandPairAttack *);
    u8 pad108[4];
    struct Player *player;
    struct SceneVec *destination; /* 0x110 */
    u8 pad114[0x118 - 0x114];
    u8 trigger[0x10]; /* 0x118: scene message receiver (StateActor receiver2). */
    u8 pad128[0x174 - 0x128];
    struct PositionedEffectCoordinates coordinates;
    struct VanargandBodyAnimation animation;
    u8 drawable[0x20C - 0x1EC];
    struct ContactPosition effectOrigin;
    u8 unknown214[8];
    struct VanargandActionSequence actionSequence; /* 0x21C */
    struct VanargandDamagePart part;
    struct MotionTrack track;
    struct VanargandAttackPair *pair;
    u8 pad570[0x580 - 0x570];
    u32 selected;
    u16 parameter584;
    u16 parameter;
    u16 parameter588;
    u16 drainDamage;
    u8 pad58C[0x59E - 0x58C];
    u16 thresholds[3];
    u32 hitFlags;
    s16 damage;
    u16 hitDuration;
    s16 accumulatedDamage;
    u16 flashTimer;
    u16 delay;
    s16 drainStartFrame;
    s16 repeats;
    u8 pad5B6[2];
    u16 attackParameter5B8, attackParameter5BA;
    u8 pad5BC[0x5C0 - 0x5BC];
    u16 attackPhase; /* 0x5C0: cleared by the mode 25 transition (tentative). */
    s16 mode;
    u8 unknown5C4[4];
    struct ContactCollider attackCollider, radialColliders[2];
    struct ContactCollider damageSource; /* Contact collider also identifies damage numbers. */
    u16 palettes[16][16];
    u8 fadeA[0xB18 - 0x918];
    u8 fadeB[0xD18 - 0xB18];
    u8 fadeC[0xF24 - 0xD18];
    u32 variant;
    u32 stageQueued
        [3]; /* 0xF28: per stage (indexed by transition), tested by the queued action state. */
    s32 transition;
    s32 attachment; /* 0xF38: passed to sub_08159164 on entering state 21. */
    u16 flash;
    u8 unknownF3E[6];
    void *afterimages;
};

static inline u32 VanargandPairAttack_TestFlags(const struct VanargandPairAttack *owner, u32 mask)
{
    return owner->flags & mask;
}

static inline void VanargandPairAttack_SetFlags(struct VanargandPairAttack *owner, u32 flags)
{
    owner->flags |= flags;
}

static inline void VanargandPairAttack_EnterPhase(struct VanargandPairAttack *owner, u32 phase)
{
    owner->phase = phase;
    owner->timer = 0;
}

/* StateActor_ConsumeActionEntry on the body. */
static inline u8 Vanargand_ConsumeActionEntry(struct VanargandPairAttack *owner)
{
    if (owner->entered) {
        owner->entered = 0;
        owner->finished = 0;
        return TRUE;
    }
    return FALSE;
}

/* Enter action animation ANIMATION: stop the running action sequence, then request the
 * animation. Some forty of the body's action states begin this way. */
/* SceneActorState_SetAnimation on the body. */
static inline void Vanargand_SetActionAnimation(struct VanargandPairAttack *owner, u32 animation)
{
    owner->requestedAnimation = animation;
}

static inline void Vanargand_EnterAction(struct VanargandPairAttack *owner, u32 animation)
{
    Vanargand_StopActionSequence(&owner->actionSequence);
    owner->requestedAnimation = animation;
}

/* SceneActorState_EnterPhase on the body: move to action phase PHASE and re-enter it. */
static inline void Vanargand_EnterActionPhase(struct VanargandPairAttack *owner, u32 phase)
{
    owner->actionPhase = phase;
    owner->actionTimer = 0;
    owner->entered = 1;
}

/* SceneActorState_StartAction on the body. */
static inline void Vanargand_StartAction(struct VanargandPairAttack *owner, u32 mode, u32 index,
                                         u32 phase)
{
    owner->actionMode = mode;
    owner->actionIndex = index;
    Vanargand_EnterActionPhase(owner, phase);
}

static inline struct VanargandAttackPair *Vanargand_GetAttackPair(struct VanargandPairAttack *owner)
{
    return owner->pair;
}

static inline u32 Vanargand_HalfState(const struct VanargandAttackPair *pair, u32 half)
{
    return half ? pair->halves[1].requestedState : pair->halves[0].requestedState;
}

void Vanargand_ResetPaletteTransition(struct VanargandPairAttack *);

/* Load the stage palette for the variant (also the leap's opening) and reset the palette
 * transition. */
static inline void Vanargand_LoadStagePalette(struct VanargandPairAttack *p,
                                              struct VanargandPairAttack *self)
{
    s32 kind = 0x6F;
    if (!p->variant)
        kind = 0x7F;
    switch (kind) {
        case 0x6F: {

            CpuSet(gPaletteBankData + 0xDE0, p->palettes, 0x04000020);
            CpuSet(p->fadeA, gBgPaletteBuffer, 0x04000068);
            Vanargand_SetStageColors(p->transition);
            break;
        }
        case 0x82:
            CpuSet(gPaletteBankData + 0x1040, p->palettes, 0x04000020);
            CpuSet(p->fadeB, gBgPaletteBuffer, 0x04000068);
            break;
        case 0x7F:
            CpuSet(gPaletteBankData + 0xFE0, p->palettes, 0x04000018);
            CpuSet(p->fadeC, gBgPaletteBuffer, 0x04000068);
            break;
        default:
            CpuSet(gPaletteBankData + 0x3240, self->palettes, 0x04000008);
            break;
    }
    Vanargand_ResetPaletteTransition(self);
}

/* Leaving reaction state 3 (the flash): restore the stage palette and stop the action
 * sequence. */
static inline void Vanargand_RestoreStagePalette(struct VanargandPairAttack *p,
                                                 struct VanargandPairAttack *self)
{
    Vanargand_LoadStagePalette(p, self);
    Vanargand_StopActionSequence(&self->actionSequence);
}

/* SceneActorState_EnterAction on the body, restoring the palette when it leaves state 3. */
static inline void Vanargand_StartReaction(struct VanargandPairAttack *p, s32 state,
                                           void (*callback)(void *, s32))
{
    struct VanargandPairAttack *self = p;
    // Keep the transition owner distinct from the callback installation owner.
    if (p->state == 3)
        Vanargand_RestoreStagePalette(p, self);
    {
        p->state = state;
        p->stateCallback = callback;
        p->callbackFrame = 0;
        p->pending = 1;
        p->phase = 0;
        p->timer = 0;
    }
}

#endif
