#ifndef GUARD_STATE_ACTOR_H
#define GUARD_STATE_ACTOR_H

#include "global.h"

/* Actors driven by a per-kind state table (gUnk_086121C4, gUnk_08612344,
 * gUnk_086124D4, ...). Each table's first 16 states are shared handlers
 * (0x0802FD60-0x08030658); the rest belong to the kind. A kind's dispatcher
 * calls table[p->state](ctx, p) each frame.
 *
 * Command states (the long runs of near-identical handlers) set the next
 * state on entry and wait for a scene message on `receiver`, registered with
 * SceneMessages_RegisterReceiver. Field names are provisional (confidence=low). */
/* x, y, z in 1/16 tile units; word-aligned, so it copies as two words */
struct StateActorPos {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));

struct StateActorTag {
    u8 unknown0[4];
    u16 field4, field6;
    union {
        u32 word;
        struct {
            u16 unknown8, fieldA;
        } halves;
    } state; /* +8; upper half cleared by 081CF84C, whole word by 081D2544. */
    u8 unknownC[0xC];
    void *attachment;       /* +0x18 */
    s32 paletteBlendFrames; /* +0x1C, decremented by 081D25AC. */
};

struct StateActor {
    u8 unk0;
    u8 kind; /* 0x01: selects animation variants (cases 4-29) */
    u8 unk2[2];
    u8 state;        /* 0x04 */
    u8 nextState;    /* 0x05 */
    u8 prevState;    /* 0x06 */
    u8 done;         /* 0x07: the current state has finished */
    u8 enterCommand; /* 0x08: entry pending (command states) */
    u8 enter;        /* 0x09: entry pending (kind states) */
    u8 unkA[4];
    u16 displayFlags;                 /* 0x0E */
    u32 stateFrame;                   /* 0x10 */
    u32 timer;                        /* 0x14 */
    u8 phase;                         /* 0x18 */
    u8 facing4, facing8, targetAngle; /* 0x19-0x1B */
    u8 unk1C[0x20 - 0x1C];
    struct StateActorTag tag; /* 0x20 */
    u8 unk40[0x54 - 0x40];
    struct StateActorPos target; /* 0x54: position the actor walks to */
    u8 motion[0x65 - 0x5C];      /* Descriptor prefix, 081D2544. */
    u8 angle;                    /* 0x65, initialized from targetAngle by 081D1A44. */
    u8 unk66[2];
    struct StateActorPos position; /* 0x68 */
    u8 unk70[0x20];                /* 0x70-0x8F: still unknown. */
    /* 0x90-0x167: a second, nested command/recovery pipeline (actor graphics
     * and basic states, 08021EA8-08024F2C), parallel to the primary state
     * machine above but with its own offsets. Field names are provisional
     * (confidence=low); several bytes are only pinned by neighbouring
     * confirmed fields and are not independently read anywhere yet. */
    u8 unk90[4];
    s16 health, maxHealth; /* 0x94: SceneActors_AddHealthCost, _TryStartDeathOrFlaggedAction. */
    u8 unk98[2];
    u8 vulnerable; /* 0x9A: set 1 by recovery states; the death/flag check clears it. */
    u8 unk9B;
    u32 actionFlags; /* 0x9C: per-frame bits; meaning differs by caller (busy/pending/variant). */
    u8 unkA0[0xA];
    u8 actionAnimation; /* 0xAA: requested sub-animation id for the recovery pipeline. */
    u8 actionState;     /* 0xAB: checked for value 3 by the variant-recovery handlers. */
    u8 actionMode;      /* 0xAC: index into SceneActorState_UpdateActionController's table. */
    u8 actionIndex;     /* 0xAD: index into the Active/Secondary/Passive action tables. */
    u8 actionPhase;     /* 0xAE: index into the phase/primary/secondary action tables. */
    u8 actionFinished;  /* 0xAF */
    u8 actionEnter;     /* 0xB0 */
    u8 actionChanged;   /* 0xB1: unconfirmed, position only. */
    u8 unkB2;
    u8 locked; /* 0xB3: SceneActorAnimation_Update{Directional,EightDirection}Callback. */
    u8 unkB4[0x10];
    u32 actionTimer; /* 0xC4: reset to 0 on transition, incremented each call otherwise. */
    u8 unkC8[4];
    u8 requestedAngle; /* 0xCC */
    u8 unkCD[0x37];
    u8 (*actionTryUpdate)(void *); /* 0x104 */
    void (*actionReset)(void *);   /* 0x108 */
    u8 unk10C[0xC];
    u8 receiver2[0x10]; /* 0x118: a second scene message receiver (size tentative). */
    u8 unk128[0x40];
    u8 receiver[0x10];                 /* 0x168: scene message receiver (size tentative). */
    u8 bodyDrawable[0x1CC - 0x178];    /* 0x178: SceneActorGraphics_InitializeBody. */
    u8 overlayDrawable[0x260 - 0x1CC]; /* 0x1CC: SceneActorGraphics_InitializeOverlay. */
    struct StateActorPos
        worldPosition; /* 0x260: render/collider position (distinct from `position`). */
    u8 unk268[0x20];
    void *palette; /* 0x288: active palette bank, picked by SceneActorGraphics_SelectResource. */
    u8 unk28C[0x2CC - 0x28C];
    void (*stateCallback)(void); /* 0x2CC */
    u8 unk2D0[0x2E4 - 0x2D0];
    s32 (*animationCallback)(struct StateActor *); /* 0x2E4 */
    u8 unk2E8[0x300 - 0x2E8];
    s32 counter300; /* Advanced by 081CFCA4; meaning tentative. */
    u8 unk304[0x343 - 0x304];
    u8 variant;         /* 0x343: SceneActorAnimation_Update{Directional,EightDirection}Callback. */
    u8 groundEffect[1]; /* 0x344: embedded projection object, full extent unknown; see
                          * include/actor_graphics_and_basic_states.h for what this unit learned. */
};

u32 Trigger_ConsumePending(void *receiver, u32 count);

/* On entry select the next state, then consume a message once done. */
#define STATE_COMMAND_WAIT(name, next)                                                             \
    void name(void *ctx, struct StateActor *p)                                                     \
    {                                                                                              \
        if (p->enterCommand != 0) {                                                                \
            p->enterCommand = 0;                                                                   \
            p->done = 0;                                                                           \
            p->nextState = next;                                                                   \
        }                                                                                          \
        if (p->done != 0)                                                                          \
            Trigger_ConsumePending(p->receiver, 1);                                                \
    }

/* As STATE_COMMAND_WAIT, counting frames in `timer`. */
#define STATE_COMMAND_WAIT_TIMED(name, next)                                                       \
    void name(void *ctx, struct StateActor *p)                                                     \
    {                                                                                              \
        if (p->enterCommand != 0) {                                                                \
            p->enterCommand = 0;                                                                   \
            p->done = 0;                                                                           \
            p->nextState = next;                                                                   \
        }                                                                                          \
        if (p->done != 0)                                                                          \
            Trigger_ConsumePending(p->receiver, 1);                                                \
        p->timer++;                                                                                \
    }

/* On entry select the next state and consume a message at once. */
#define STATE_COMMAND_GO(name, next)                                                               \
    void name(void *ctx, struct StateActor *p)                                                     \
    {                                                                                              \
        if (p->enterCommand != 0) {                                                                \
            p->enterCommand = 0;                                                                   \
            p->done = 0;                                                                           \
            p->nextState = next;                                                                   \
            Trigger_ConsumePending(p->receiver, 1);                                                \
        }                                                                                          \
    }

/* As STATE_COMMAND_GO, counting frames in `timer`. */
#define STATE_COMMAND_GO_TIMED(name, next)                                                         \
    void name(void *ctx, struct StateActor *p)                                                     \
    {                                                                                              \
        if (p->enterCommand != 0) {                                                                \
            p->enterCommand = 0;                                                                   \
            p->done = 0;                                                                           \
            p->nextState = next;                                                                   \
            Trigger_ConsumePending(p->receiver, 1);                                                \
        }                                                                                          \
        p->timer++;                                                                                \
    }

/* On entry select the next state; once done return to state 1 (idle). */
#define STATE_COMMAND_RESTART(name, next)                                                          \
    void name(void *ctx, struct StateActor *p)                                                     \
    {                                                                                              \
        if (p->enterCommand != 0) {                                                                \
            p->enterCommand = 0;                                                                   \
            p->done = 0;                                                                           \
            p->nextState = next;                                                                   \
        }                                                                                          \
        if (p->done != 0) {                                                                        \
            u32 o = 1;                                                                             \
            p->state = o;                                                                          \
            p->timer = 0;                                                                          \
            p->enterCommand = o;                                                                   \
            Trigger_ConsumePending(p->receiver, 1);                                                \
        }                                                                                          \
    }

/* On entry select `next`; once done, coming back from `next` itself go to
 * `again`, else consume a message. */
#define STATE_COMMAND_REPEAT(name, next, again)                                                    \
    void name(void *ctx, struct StateActor *p)                                                     \
    {                                                                                              \
        if (p->enterCommand) {                                                                     \
            p->enterCommand = 0;                                                                   \
            p->done = 0;                                                                           \
            p->nextState = next;                                                                   \
        }                                                                                          \
        if (p->done) {                                                                             \
            if (p->prevState == next)                                                              \
                p->nextState = again;                                                              \
            else                                                                                   \
                Trigger_ConsumePending(p->receiver, 1);                                            \
        }                                                                                          \
    }

#endif
