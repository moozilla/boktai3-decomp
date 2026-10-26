// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "game_state.h"
#include "ram.h"

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
void ScenePanelEffect_CreatePositionEffect(struct NidhoggrEffectVector *, void *);

static inline void StepSequence(struct NidhoggrAnimatedSlot *owner)
{
    struct NidhoggrPoolSprite *sprite = &owner->sprite;
    struct NidhoggrPoolSequence *state = &owner->sequence;
    const volatile u16 *command = state->commands + state->index;
    u32 duration, flags, bit;
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 one = 1;
        bit = one;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit = 2;
        register u32 first asm("r1");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        u8 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        asm("" : "+r"(mask));
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold)
                        state->elapsed = savedDuration;
                    else
                        state->index = state->frameCount - 1;
                } else {
                    state->index = index - 1;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        state->index = frameCount - 1;
                    } else
                        state->index = 0;
                }
            }
            command = state->commands + state->index;
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        }
    }
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
void Nidhoggr_UpdateOwnedSeekingEffect(struct NidhoggrAnimatedSlot *owner)
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
            asm("" : : "r"(targetSlot), "r"(stateOffset));
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
        ScenePanelEffect_CreatePositionEffect((struct NidhoggrEffectVector *)&impact, boss);
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
            ScenePanelEffect_CreatePositionEffect((struct NidhoggrEffectVector *)&impact, boss);
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
