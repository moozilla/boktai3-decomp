// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "game_state.h"
#include "ram.h"
#include "player_cue_sequence.h"

struct NidhoggrPoolSequence {
    const volatile u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unk_A, speed, elapsed;
};
struct NidhoggrEffectVector {
    u16 x, y, z, unk_6;
};
struct NidhoggrPoolSprite {
    u32 flags;
    u8 unk_4[12];
    u16 frame;
    u8 unk_12[10];
    struct NidhoggrEffectVector position;
    u8 unk_24[8];
};
struct NidhoggrAnimatedSlot {
    void *update, *onComplete;
    u8 animationData[0x1c];
    struct NidhoggrPoolSprite sprite;
    struct NidhoggrPoolSequence sequence;
    u8 unk_60[0x178 - 0x60];
    struct NidhoggrEffectVector motion;
    u8 state, age;
    s16 unk_182;
    void *boss;
};
struct NidhoggrTarget {
    u8 unk_0[0x30];
    s16 x, y, z;
};
// Packed coordinates preserve the unused upper halfword of the last word.
struct EffectDelta {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
extern const u16 gUnk_086141BC[];
extern const s16 gUnk_086149C4[];
s32 Callback_InvokeActor(void *, void *);
s32 Math_Atan2(s32, s32);
void Nidhoggr_CreateTimedPositionEffect(struct NidhoggrEffectVector *, void *);

void Collider_Remove(void *);

static inline void StepSequence(struct NidhoggrAnimatedSlot *owner)
{
    PlayerCue_StepSequence(&owner->sprite, &owner->sequence);
}

static inline s32 RandomValue(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
}
static inline void AddPosition(struct NidhoggrEffectVector *out,
                               struct NidhoggrEffectVector *position)
{
    out->x += position->x;
    out->y += position->y;
    out->z += position->z;
}
// Tentative: an owned Nidhoggr effect that seeks the player before impact.
#define Nidhoggr_UpdateSeekingAttackEffect sub_0818FEE4
void Nidhoggr_UpdateSeekingAttackEffect(struct NidhoggrAnimatedSlot *owner)
{
    struct EffectDelta delta;
    s32 dx, dy, dz, angle;
    register u32 random asm("r5");
    u8 *stateAddress;
    register u32 stateOffset;
    StepSequence(owner);
    stateOffset = (u32) & ((struct NidhoggrAnimatedSlot *)0)->state;
    stateAddress = (u8 *)owner + stateOffset;
    if (*stateAddress == 0)
        goto waiting;
    if (*stateAddress == 255)
        goto impact;
    {
        {
            struct NidhoggrTarget **targetSlot = &gGameState;
            struct NidhoggrTarget *target = *targetSlot;
            s32 targetX = target->x;
            /* Keep both address values live through the signed coordinate load. */
            dx = targetX - (s16)owner->sprite.position.x;
            dy = target->y - (s16)owner->sprite.position.y;
            dz = target->z - (s16)owner->sprite.position.z;
        }
        angle = Math_Atan2(dx, dz);
        if (RandomValue() > 0xdfff)
            goto fastMotion;
        delta.x = gUnk_086149C4[(angle + 64) & 255] >> 10;
        delta.y = 0;
        delta.z = gUnk_086149C4[angle & 255] >> 10;
        goto spread;
    waiting: {
        if ((s16)++owner->motion.x > 8)
            owner->motion.x = 8;
        if (owner->age > 30) {
            (*stateAddress)++;
            owner->age = 0;
        }
    }
        return;
    impact: {
        struct EffectDelta impact;
        void *boss = owner->boss;
        impact.x = 0;
        impact.y = -96;
        impact.z = 0;
        AddPosition((struct NidhoggrEffectVector *)&impact, &owner->sprite.position);
        Nidhoggr_CreateTimedPositionEffect((struct NidhoggrEffectVector *)&impact, boss);
        owner->update = 0;
        if (owner->onComplete) {
            Callback_InvokeActor(owner, owner->onComplete);
            owner->onComplete = 0;
        }
    }
        return;
    fastMotion:
        delta.x = gUnk_086149C4[(angle + 64) & 255] >> 9;
        delta.y = 0;
        delta.z = gUnk_086149C4[angle & 255] >> 9;
    spread:
        random = 255 & RandomValue();
        delta.x += gUnk_086149C4[(random + 64) & 255] >> 10;
        delta.z += gUnk_086149C4[random & 255] >> 10;
        owner->motion = *(struct NidhoggrEffectVector *)&delta;
        if (owner->age == 0)
            owner->state++;
        if (owner->state > 1 && owner->age > 88) {
            struct EffectDelta impact;
            void *boss = owner->boss;
            impact.x = 0;
            impact.y = -128;
            impact.z = 0;
            AddPosition((struct NidhoggrEffectVector *)&impact, &owner->sprite.position);
            Nidhoggr_CreateTimedPositionEffect((struct NidhoggrEffectVector *)&impact, boss);
            Collider_Remove((u8 *)owner + 0x120);
            owner->update = 0;
            if (owner->onComplete) {
                Callback_InvokeActor(owner, owner->onComplete);
                owner->onComplete = 0;
            }
        } else {
            dx += 48;
            dy += 256;
            dz += 48;
            if ((u32)dx <= 96 && dy >= 0 && dy <= 256 && dz >= 0 && dz <= 96)
                owner->state = 255;
        }
    }
}
