#include "global.h"
#include "display_animation.h"
#include "display_sequence_inline.h"

// MATCHING_CONCESSION: register-pinning
// Allocate an attack particle and initialize its sequence when requested.
#define PlayerAttackEffect_AllocateParticle sub_081606C0
struct AttackParticleSlot {
    u32 life, sequence, mode;
    s32 angle;
};
struct AttackParticleOwner {
    u8 unknown0[0x20];
    struct ContactPosition position;
};
/* PlayerAttackEffect_Create allocates 0x5A8 bytes. Its initializer sets up sixteen slots. */
struct AttackParticleEffect {
    u8 taskAndGraphics[0xC4];
    struct DisplaySequenceSprite sprites[16];
    struct DisplaySequenceState sequences[16];
    struct AttackParticleSlot slots[16];
    u32 frame, unknown588;
    const u8 *facing, *side;
    const struct AttackParticleOwner *owner;
    u32 previous, current, element;
    void *sequenceResource;
};
#define EFFECT_OFFSET(field) ((u32) & ((struct AttackParticleEffect *)0)->field)
s32 Math_Atan2(s32, s32);
void Sprite_ApplyAnimation(struct DisplaySequenceSprite *, const u8 *);
u32 Anim_RestartSequence(u8 *, u32, u16, u8, u8);
static inline u32 AdvanceParticleSequence(struct DisplaySequenceSprite *sprite,
                                          struct DisplaySequenceState *frames, u32 one)
{
    struct DisplaySequenceState *state = frames;
    register u32 maskBit asm("r9") = one;
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u32 flags;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r2");
        first = maskBit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & maskBit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r2");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        duration = state->duration;
        elapsed = (u16)elapsed;
        temp = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = temp;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = 8;
                    endFlag &= flags;
                    if ((u8)endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    ended = 1;
                } else {
                    ended = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)(((u32)table) + offset);
            }
            DisplaySequence_UpdateWeight(state, (const u16 *)command);
            if (!state->duration)
                state->duration = 1;
        } else {
            ended = 0;
        }
    }
    return ended;
}

s32 PlayerAttackEffect_AllocateParticle(u8 *arg, struct ContactPosition *pos, s32 lifetime,
                                        void *anim, s32 sequence, s32 kind)
{
    u8 *p = arg;
    struct AttackParticleEffect *effect = (void *)arg;
    struct ContactPosition *position = pos;
    s32 remaining = lifetime;
    void *animation = anim;
    register s32 mode asm("r9") = kind;
    s32 i;
    register s32 increment asm("r3");
    s32 check;
    struct AttackParticleSlot *slot = effect->slots;
    struct DisplaySequenceState *state;
    i = 0;
    state = effect->sequences;
    do {
        if (slot->life == 0) {
            s32 offset = sizeof(struct DisplaySequenceSprite) * i;
            struct DisplaySequenceSprite *sprite =
                (void *)((offset + (u32)p) + EFFECT_OFFSET(sprites));
            s32 angle;
            u8 *positions;
            sprite->flags &= ~1;
            positions = p + EFFECT_OFFSET(sprites[0].position);
            positions += offset;
            *((struct ContactPosition *)positions) = *position;
            slot->life = remaining;
            Sprite_ApplyAnimation(
                (void *)(((sizeof(struct DisplaySequenceSprite) * i) + EFFECT_OFFSET(sprites)) + p),
                animation);
            slot->sequence = sequence;
            slot->mode = mode;
            angle = Math_Atan2(
                position->x -
                    (*(const struct AttackParticleOwner **)(p + EFFECT_OFFSET(owner)))->position.x,
                position->z -
                    (*(const struct AttackParticleOwner **)(p + EFFECT_OFFSET(owner)))->position.z);
            slot->angle = angle;
            if (*(*((u8 **)(p + EFFECT_OFFSET(side)))))
                slot->angle = angle + 64;
            else
                slot->angle = angle - 64;
            slot->angle &= 255;
            {
                register s32 modeCheck asm("r3") = mode;
                asm("" : "+r"(modeCheck));
                if (modeCheck == 1) {
                    register u32 resourceOffset asm("r1") = EFFECT_OFFSET(sequenceResource);
                    register void **resourceSlot asm("r0");
                    void *resource;
                    asm("" : "+r"(resourceOffset));
                    resourceSlot = (void **)(p + resourceOffset);
                    resource = *resourceSlot;
                    Anim_RestartSequence((u8 *)state, (u32)resource, (u16)sequence, 0, 0);
                    AdvanceParticleSequence(
                        (void *)(((sizeof(struct DisplaySequenceSprite) * i) + p) +
                                 EFFECT_OFFSET(sprites)),
                        state, mode);
                }
            }
            return 1;
        }
        state++;
        increment = 1;
        asm("" : "+r"(increment));
        i += increment;
        slot++;
        check = i;
        asm("" : "+r"(check));
    } while (check < 16);

    return 0;
}
