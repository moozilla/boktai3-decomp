#include "global.h"
#include "ratatosk.h"
#include "engine.h"
#include "ram.h"

#define W(o) (*(u32 *)((u8 *)p2 + (o)))
void Bestiary_UnlockEntry(s32);
extern const struct ContactPosition gUnk_0824FDF0;
struct ContactPosition *Ratatosk_GetRandomMovementPoint(void *);
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void Ratatosk_UpdateDefeatSequence(void);
void sub_081F5290(void);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void Sound_PlaySongChecked(u32);
static inline u32 randomTable(u32 table, u32 *index, u32 fullMask)
{
    *index = (*index + 1) & fullMask;
    return ((const u16 *)table)[*index];
}
static inline u32 randomB(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
}
static inline void setState(struct Ratatosk *p, void (*update)(void), u8 state)
{
    p->actionState = state;
    p->actionCallback = update;
    p->callbackFrame = 0;
    p->actionChanged = 1;
    p->reactionPhase = 0;
    p->reactionTimer = 0;
}
static inline u32 testFlags(struct Ratatosk *p, u32 mask)
{
    return p->flags & mask;
}
// confidence=low: consumes queued damage and transitions to recoil or defeat.
#define Ratatosk_ApplyQueuedDamage sub_081F6428
s32 Ratatosk_ApplyQueuedDamage(struct Ratatosk *p)
{
    struct {
        struct ContactPosition spawn, velocity;
    } scratch;
    struct Ratatosk *p2 = p;
    s32 react = 0;
    s32 damage = (s16)p2->frameDamage;
    u32 particles = p2->motionFlag;
    s32 oldPhase;
    u32 changed;
    p2->frameDamage = react;
    p2->unknownE71 = 0;
    p2->motionFlag = 0;
    oldPhase = p2->healthPhase;
    if (!p2->healthPhase &&
        p->health < ((p->maxHealth >> 1) + (p->maxHealth >> 3) + (p->maxHealth >> 4)))
        p2->healthPhase = 1;
    else if (p2->healthPhase <= 1 &&
             p->health < ((p->maxHealth >> 2) + (p->maxHealth >> 4) + (p->maxHealth >> 5)))
        p2->healthPhase = 2;
    if (oldPhase != p2->healthPhase) {
        p2->destination = *Ratatosk_GetRandomMovementPoint(p2);
        p2->savedState = 2;
    }
    if (damage > 0) {
        if (testFlags(p, 16) || p2->interruptLock > 0) {
            react = 1;
            if (!Player_IsSlotInActionTwelveOrThirtyEight(0))
                damage *= 3;
            if (damage > 500)
                damage = (randomB() & 31) + 484;
            p2->interruptLock = 0;
        }
        p->health -= damage;
        p2->damageTimer = 8;
        if (p->health <= 0) {
            p->health = 0;
            {
                u32 mask = 2;
                changed = p->flags | mask;
                p->flags = changed;
            }
            {
                void (*update)(void);
                u32 animation;
                changed = 1;
                update = Ratatosk_UpdateDefeatSequence;
                animation = 15;
                p->actionState = animation;
                p->actionCallback = update;
                p->callbackFrame = 0;
                p->actionChanged = changed;
                p->reactionPhase = 0;
                p->reactionTimer = 0;
            }
            Bestiary_UnlockEntry(1);
            return 1;
        }
        if (damage > 0)
            DamageNumberDisplay_Show((u8 *)p2 + 0x188, damage, 0, (u8 *)p2 + 0x194,
                                     (u8 *)p2 + 0x19c, 1);
        if (particles) {
            s32 i;
            u8 *angle;
            struct ContactPosition *vel;
            struct ContactPosition *pos;
            u32 *index;
            u32 fullMask, mask;
            vel = &scratch.velocity;
            angle = &p->body.direction;
            pos = &scratch.spawn;
            index = &gRandomIndex;
            fullMask = 1023;
            mask = 127;
            i = 2;
            do {
                u32 sample, x;
                u32 value;
                scratch.velocity = gUnk_0824FDF0;
                scratch.spawn = p->body.position;
                value = (u32)gRandomTable;
                sample = randomTable(value, index, fullMask);
                x = (u16)pos->x - 64;
                sample &= mask;
                x += sample;
                pos->x = x;
                pos->y += (u8)randomTable(value, index, fullMask);
                sample = randomTable(value, index, fullMask);
                x = (u16)pos->z - 64;
                sample &= mask;
                x += sample;
                pos->z = x;
                value = *angle;
                value += 128;
                SparkParticle_Spawn(&scratch.spawn, 1, vel, value, 3, 8, 2, 2);
            } while (--i >= 0);
        }
        if (react) {
            u32 one;
            p2->savedPhase = p2->actionCounter;
            p2->savedTime = p->callbackFrame;
            p2->savedState = p->actionPhase;
            p2->savedSequence = W(0x180);
            one = 1;
            p->flags |= one;
            setState(p, sub_081F5290, 18);
            return 1;
        }
        Sound_PlaySongChecked(318);
    }
    return 0;
}
