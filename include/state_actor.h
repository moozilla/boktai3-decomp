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
    u16 displayFlags; /* 0x0E */
    u32 stateFrame;   /* 0x10 */
    u32 timer;        /* 0x14 */
    u8 phase;         /* 0x18 */
    u8 unk19[0x54 - 0x19];
    struct StateActorPos target; /* 0x54: position the actor walks to */
    u8 unk5C[0x68 - 0x5C];
    struct StateActorPos position; /* 0x68 */
    u8 unk70[0x168 - 0x70];
    u8 receiver[0x2CC - 0x168];  /* 0x168: scene message receiver */
    void (*stateCallback)(void); /* 0x2CC */
};

void Trigger_ConsumePending(void *receiver, s32 count);

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
