#include "global.h"
#include "display_sequence_inline.h"

// confidence=low: updates attack particles and advances their animation or lifetime.
#define PlayerAttackEffect_UpdateParticles sub_0816233C
#include "contact_swarm.h"
extern void (*const gUnk_08610E2C[])(u8 *);
extern const s16 gUnk_086149C4[];
struct Vec {
    u16 x, y, z, pad;
};
static inline s32 Divide4096(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
static inline void CircleVector(struct Vec *vector, s32 angle, s32 radius)
{
    vector->x = Divide4096(gUnk_086149C4[(angle + 64) & 255] * radius);
    vector->y = 0;
    vector->z = Divide4096(gUnk_086149C4[angle & 255] * radius);
}
struct ParticleSlot {
    u32 life, sequence, mode;
    s32 angle;
};
struct ParticleEffect {
    u8 gap[0xc4];
    struct ContactSwarmSprite sprites[16];
    struct DisplaySequenceState sequences[16];
    struct ParticleSlot slots[16];
    u32 frame, unknown588;
    u8 *direction, *rotation, *owner;
    u32 previous, current, element;
    void *resource;
};
struct SpriteRef {
    struct ContactSwarmSprite *sprite;
};
static inline struct SpriteRef SpriteRef_FromSprite(struct ContactSwarmSprite *sprite)
{
    struct SpriteRef ref;
    ref.sprite = sprite;
    return ref;
}
static inline u32 AddParticleFrame(u32 base, u32 frame)
{
    return frame + base;
}
static inline u32 StepParticles(struct ContactSwarmSprite *owner,
                                struct DisplaySequenceState *state, u32 one)
{
    u32 ended;
    const u16 *command;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = one;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
                ended = 1;
            } else {
                state->index--;
                ended = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = one;
    } else {
        ended = 0;
    }
    return ended;
}
void PlayerAttackEffect_UpdateParticles(struct ParticleEffect *p)
{
    struct Vec delta;
    register s32 i;
    struct Vec *motion;
    struct ContactSwarmSprite *sprite;
    u32 one;
    Callback_InvokeActor(p, gUnk_08610E2C[p->element]);
    p->previous = p->current;
    p->current = 0;
    i = 0;
    one = 1;
    motion = &delta;
    for (sprite = p->sprites; i < 16; sprite++, i++) {
        struct ParticleSlot *slot = &p->slots[i];
        u32 old = slot->life;
        if (old == 0)
            sprite->flags |= one;
        else {
            u32 time = old - 1;
            slot->life = time;
            if (slot->mode == 0) {
                u32 frame = 0;
                if (time == 0) {
                    slot->life = frame;
                    sprite->flags |= one;
                } else {
                    frame = old - 2;
                    if (frame > 2)
                        frame = 2;
                    sprite->frame = AddParticleFrame(slot->sequence, frame);
                }
            } else if (StepParticles(SpriteRef_FromSprite(sprite).sprite, &p->sequences[i], one)) {
                sprite->flags |= one;
                slot->life = 0;
            } else {

                slot->life = 5;
                if (*p->rotation)
                    slot->angle -= 60;
                else
                    slot->angle += 60;
                CircleVector(&delta, slot->angle, 16);
                sprite->position.x += motion->x;
                sprite->position.y += motion->y;
                sprite->position.z += motion->z;
            }
        }
    }
}
