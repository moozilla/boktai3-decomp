#include "global.h"
#include "ram.h"

struct Vec {
    s16 x, y, z, pad;
};
struct Position {
    u32 xy, z;
};
struct Actor {
    u8 pad0[0x54];
    struct Vec position;
    u8 pad5c[0x34];
    u32 timer;
    u8 pad94[8];
    u32 flags;
    u8 padA0[0xe];
    u8 state, ready, active;
    u8 padB1[0x13];
    u32 stateCounter;
    u8 padC8[0xb4];
    u32 tick, sequence, complete;
    u8 pad188[0x1e4];
    struct Vec anchor;
    u8 pad374[0xae8];
    struct Position destination;
    u8 padE64[8];
    u16 duration;
    u8 padE6e[0x12];
    s16 interruptLock;
    u8 padE82[6];
    s16 hasOrb;
    u8 padE8a[4];
    s16 pattern, choice;
    u8 padE92[4];
    s16 healthPhase;
    u8 padE98[0x50];
    u32 savedTick, savedTimer, savedState, savedSequence;
    s16 pendingAttack, gate;
    u8 padEfc[8];
    u32 interval;
    u8 padF08[0x24];
    s32 orbManager;
};
struct ChoiceTable {
    s32 entries[1][4];
};
extern const struct ChoiceTable gUnk_0824FDF8, gUnk_0824FE08, gUnk_0824FE18;
extern void *const gUnk_08613F04[];
extern const u16 gUnk_0203B400[];
u8 Callback_InvokeActor(void *, void *);
struct Vec *Player_GetEffectivePosition(s32);
void Ratatosk_BeginInterruptState(void *);
void Sound_StopSong(u32);
void PaletteEffect_RefreshPalette(void *);
struct Position *Ratatosk_GetRandomMovementPoint(void *);
void Ratatosk_GetPlayerOffsetFarthestFromSelf(void *, void *);
void SunOrb_FadeOutAll(s32);
static inline u32 flags(struct Actor *p, u32 mask)
{
    return p->flags & mask;
}
static inline u32 randomB(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gUnk_0203B400[gRandomIndex];
}
#define RANDOM_TABLE_ADDR 0x0203B400
static inline s32 randomByte(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u8 *)(RANDOM_TABLE_ADDR + gRandomIndex * 2);
}
static inline void snapshot(struct Actor *p, struct Actor *q)
{
    p->savedTick = p->tick;
    p->savedTimer = q->timer;
    p->savedState = q->state;
    p->savedSequence = p->sequence;
}
static inline void snapshotWithState(struct Actor *p, struct Actor *q, u32 *savedState)
{
    p->savedTick = p->tick;
    p->savedTimer = q->timer;
    *savedState = q->state;
    p->savedSequence = p->sequence;
}
static inline void setState(struct Actor *q, u32 state, u32 zero, u32 one)
{
    q->state = state;
    q->stateCounter = zero;
    q->active = 1;
}
static inline void choose(struct Actor *p, struct Actor *q, struct ChoiceTable *choices)
{
    snapshot(p, q);
    setState(q, choices->entries[p->pattern][p->choice], 0, 1);
    ++p->choice;
    if (choices->entries[p->pattern][p->choice] == -1) {
        p->pattern = 0;
        p->choice = 0;
    }
}
// confidence=low: runs the active callback and chooses attacks according to health phase and interrupts.
#define Ratatosk_UpdateAttackSelection sub_081F92C8
void Ratatosk_UpdateAttackSelection(struct Actor *p)
{
    struct Actor *q = p;
    struct ChoiceTable choices;
    u32 *savedState;
    Callback_InvokeActor(p, gUnk_08613F04[q->state]);
    if (flags(p, 0x100000) && q->state == 6 && !p->interruptLock) {
        struct Vec *player = Player_GetEffectivePosition(0);
        struct Vec *anchor = &p->anchor;
        s32 dx = player->x - anchor->x;
        s32 dz = player->z - anchor->z;
        if (dx * dx + dz * dz <= 0x3ffff)
            Ratatosk_BeginInterruptState(p);
    }
    if (!flags(q, 2) && !flags(q, 1) && !p->pendingAttack) {
        if (flags(q, 0x400000) && p->healthPhase > 0 && q->state != 4 && q->state != 5) {
            if (q->state != 6 || p->hasOrb == 1) {
                snapshot(p, q);
                setState(q, 10, 0, 1);
                Sound_StopSong(0x570);
            } else
                Ratatosk_BeginInterruptState(p);
        }
        if (flags(q, 0x200000) && p->healthPhase > 0 && q->state != 4 && q->state != 5) {
            if (q->state != 6 || p->hasOrb == 1) {
                snapshot(p, q);
                setState(q, 9, 0, 1);
                Sound_StopSong(0x570);
            } else
                Ratatosk_BeginInterruptState(p);
        }
    }
    if (p->complete) {
        PaletteEffect_RefreshPalette(p);
        p->gate = 0;
        if (p->healthPhase == 0) {
            if (q->state == 2) {
                choices = gUnk_0824FDF8;
                choose(p, q, &choices);
            } else if (q->state == 1) {
                if (p->interval != 60)
                    p->duration = 32;
                if ((s32)randomByte() > 128 && p->savedState != 3)
                    setState(q, p->savedState, 0, 1);
                else {
                    p->destination = *Ratatosk_GetRandomMovementPoint(p);
                    setState(q, 2, 0, 1);
                }
                snapshot(p, q);
            } else {
                p->duration = 128;
                snapshot(p, q);
                p->destination = *Ratatosk_GetRandomMovementPoint(p);
                setState(q, 2, 0, 1);
            }
        } else if (p->healthPhase == 1) {
            if (q->state == 2) {
                choices = gUnk_0824FE08;
                choose(p, q, &choices);
            } else if ((u8)(q->state - 8) <= 2 && *(savedState = &p->savedState) == 6 &&
                       p->hasOrb == 0) {
                snapshotWithState(p, q, savedState);
                setState(q, 6, 0, 1);
            } else {
                SunOrb_FadeOutAll(p->orbManager);
                if (q->state == 1) {
                    if (p->pendingAttack) {
                        p->duration = 32;
                        snapshot(p, q);
                        setState(q, 6, 0, 1);
                        p->pendingAttack = 0;
                    } else {
                        p->duration = 32;
                        if ((s32)randomByte() > 128)
                            setState(q, p->savedState, 0, 1);
                        else {
                            p->destination = *Ratatosk_GetRandomMovementPoint(p);
                            setState(q, 2, 0, 1);
                        }
                        snapshot(p, q);
                    }
                } else {
                    p->duration = 64;
                    snapshot(p, q);
                    Ratatosk_GetPlayerOffsetFarthestFromSelf(p, &p->destination);
                    setState(q, 2, 0, 1);
                }
            }
        } else {
            if (q->state == 2) {
                choices = gUnk_0824FE18;
                choose(p, q, &choices);
            } else if ((u8)(q->state - 8) <= 2 && *(savedState = &p->savedState) == 6 &&
                       p->hasOrb == 0) {
                snapshotWithState(p, q, savedState);
                setState(q, 6, 0, 1);
            } else if (q->state == 1) {
                if (p->pendingAttack) {
                    p->duration = 32;
                    snapshot(p, q);
                    setState(q, 6, 0, 1);
                    p->pendingAttack = 0;
                } else {
                    p->duration = 32;
                    savedState = &p->savedState;
                    setState(q, *savedState, 0, 1);
                    snapshotWithState(p, q, savedState);
                }
            } else {
                SunOrb_FadeOutAll(p->orbManager);
                p->duration = 64;
                snapshot(p, q);
                Ratatosk_GetPlayerOffsetFarthestFromSelf(p, &p->destination);
                setState(q, 2, 0, 1);
            }
        }
        p->complete = 0;
    }
}
