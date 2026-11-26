#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "engine/shared/engine_globals.h"
#include "engine/shared/ram.h"

struct Vec {
    s16 x, y, z, pad;
};
struct ChoiceTable {
    s32 entries[1][4];
};
extern const struct ChoiceTable gUnk_0824FDF8, gUnk_0824FE08, gUnk_0824FE18;
extern void *const gUnk_08613F04[];
u8 Callback_InvokeActor(void *, void *);
struct Vec *Player_GetEffectivePosition(s32);
void Ratatosk_BeginInterruptState(void *);
void Sound_StopSong(u32);
void PaletteEffect_RefreshPalette(void *);
struct ContactPosition *Ratatosk_GetRandomMovementPoint(void *);
void Ratatosk_GetPlayerOffsetFarthestFromSelf(void *, void *);
void SunOrb_FadeOutAll(s32);
static inline u32 flags(struct Ratatosk *p, u32 mask)
{
    return p->flags & mask;
}
static inline u32 randomB(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gRandomTable[gRandomIndex];
}
#define RANDOM_TABLE_ADDR 0x0203B400
static inline s32 randomByte(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u8 *)(RANDOM_TABLE_ADDR + gRandomIndex * 2);
}
static inline void snapshot(struct Ratatosk *p, struct Ratatosk *q)
{
    p->savedPhase = p->actionCounter;
    p->savedTime = q->callbackFrame;
    p->savedState = q->actionPhase;
    p->savedSequence = p->sequence;
}
static inline void snapshotWithState(struct Ratatosk *p, struct Ratatosk *q, u32 *savedState)
{
    p->savedPhase = p->actionCounter;
    p->savedTime = q->callbackFrame;
    *savedState = q->actionPhase;
    p->savedSequence = p->sequence;
}
static inline void setState(struct Ratatosk *q, u32 state, u32 zero, u32 one)
{
    q->actionPhase = state;
    q->actionTimer = zero;
    q->actionEnter = 1;
}
static inline void choose(struct Ratatosk *p, struct Ratatosk *q, struct ChoiceTable *choices)
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
void Ratatosk_UpdateAttackSelection(struct Ratatosk *p)
{
    struct Ratatosk *q = p;
    struct ChoiceTable choices;
    u32 *savedState;
    Callback_InvokeActor(p, gUnk_08613F04[q->actionPhase]);
    if (flags(p, 0x100000) && q->actionPhase == 6 && !p->interruptLock) {
        struct Vec *player = Player_GetEffectivePosition(0);
        struct Vec *anchor = &p->anchor;
        s32 dx = player->x - anchor->x;
        s32 dz = player->z - anchor->z;
        if (dx * dx + dz * dz <= 0x3ffff)
            Ratatosk_BeginInterruptState(p);
    }
    if (!flags(q, 2) && !flags(q, 1) && !p->pendingAttack) {
        if (flags(q, 0x400000) && p->healthPhase > 0 && q->actionPhase != 4 &&
            q->actionPhase != 5) {
            if (q->actionPhase != 6 || p->hasOrb == 1) {
                snapshot(p, q);
                setState(q, 10, 0, 1);
                Sound_StopSong(0x570);
            } else
                Ratatosk_BeginInterruptState(p);
        }
        if (flags(q, 0x200000) && p->healthPhase > 0 && q->actionPhase != 4 &&
            q->actionPhase != 5) {
            if (q->actionPhase != 6 || p->hasOrb == 1) {
                snapshot(p, q);
                setState(q, 9, 0, 1);
                Sound_StopSong(0x570);
            } else
                Ratatosk_BeginInterruptState(p);
        }
    }
    if (p->sequenceDone) {
        PaletteEffect_RefreshPalette(p);
        p->gate = 0;
        if (p->healthPhase == 0) {
            if (q->actionPhase == 2) {
                choices = gUnk_0824FDF8;
                choose(p, q, &choices);
            } else if (q->actionPhase == 1) {
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
            if (q->actionPhase == 2) {
                choices = gUnk_0824FE08;
                choose(p, q, &choices);
            } else if ((u8)(q->actionPhase - 8) <= 2 && *(savedState = &p->savedState) == 6 &&
                       p->hasOrb == 0) {
                snapshotWithState(p, q, savedState);
                setState(q, 6, 0, 1);
            } else {
                SunOrb_FadeOutAll(p->sunOrbs);
                if (q->actionPhase == 1) {
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
            if (q->actionPhase == 2) {
                choices = gUnk_0824FE18;
                choose(p, q, &choices);
            } else if ((u8)(q->actionPhase - 8) <= 2 && *(savedState = &p->savedState) == 6 &&
                       p->hasOrb == 0) {
                snapshotWithState(p, q, savedState);
                setState(q, 6, 0, 1);
            } else if (q->actionPhase == 1) {
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
                SunOrb_FadeOutAll(p->sunOrbs);
                p->duration = 64;
                snapshot(p, q);
                Ratatosk_GetPlayerOffsetFarthestFromSelf(p, &p->destination);
                setState(q, 2, 0, 1);
            }
        }
        p->sequenceDone = 0;
    }
}
