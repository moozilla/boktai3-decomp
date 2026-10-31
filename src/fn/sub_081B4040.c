#include "global.h"
// confidence=low: action role inferred from local animation, motion and effects.
#include "contact_swarm.h"
#include "display_sequence_inline.h"

struct VanargandPart {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x1c];
    struct DisplaySequenceState sequence;
    u8 motion[0x38];
    struct ContactPosition *basePosition;
    u8 unknown94[8];
    struct ContactPosition origin;
    struct ContactPosition target;
    s16 heading, distance, particleAngle;
    u8 unknownB2[0xaa];
    u32 variant, flags, state, timer;
    u8 currentAction, previousAction, reset, enter, finished;
    u8 unknown171[7];
    u32 actionTimer;
    u8 unknown17C[0x20];
    u32 flashTimer;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
extern const s16 gUnk_086149C4[];
void *VanargandEffect_Create(s32, s32, void *, void *, u32);
void ActorStatus_Deactivate(void *, void *);
void Sound_PlaySong(u32);
void CpuSet(const void *, void *, u32);
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void Vector(struct ContactPosition *v, s32 angle, s32 scale)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * scale);
}
static inline void Add(struct ContactPosition *v, struct ContactPosition *a,
                       struct ContactPosition *b)
{
    v->x = b->x + a->x;
    v->y = b->y + a->y;
    v->z = b->z + a->z;
}
// confidence=low: emits six surrounding effects before retiring the part.
static inline void SetState(struct VanargandPart *p, u32 state)
{
    p->state = state;
    p->timer = 0;
}
static inline u32 StepSequence(struct VanargandPart *owner)
{
    struct DisplaySequenceState *state;
    const u16 *command;
    u32 ended;
    state = &owner->sequence;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = owner->sequence.commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->sprite.frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
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
            state->duration = 1;
    } else {
        ended = 0;
    }
    return ended;
}

struct VanargandPartOwner {
    u8 unknown0[0x4D0];
    void *sequenceResource;
};
void Vanargand_EmitRadialPartEffects(struct VanargandPartOwner *boss, struct VanargandPart *p)
{
    u8 *enter;
    u32 mask = 12;
    p->flags |= mask;
    p->flags |= 128;
    if (p->reset) {
        struct ContactPosition zero;
        struct ContactPosition point;
        u32 *state;
        u32 *timer;
        s32 i;
        p->reset = 0;
        Anim_SetSequence(&p->sequence, boss->sequenceResource, 10, (u8)p->variant, 0);
        ZeroFill8(&zero);
        i = 0;
        state = &p->state;
        timer = &p->timer;
        enter = &p->enter;
        for (; i < 6; i++) {
            Vector(&point, (i * 255) / 6, 128);
            Add(&point, &p->sprite.position, &point);
            VanargandEffect_Create(3, i * 2, &point, &zero, p->variant);
        }

        *state = 0;
        *timer = 0;
        *enter = 1;
    }
    switch (p->state) {
        case 0: {
            u32 entered = p->enter;
            enter = &p->enter;
            if (entered) {
                *enter = 0;
                p->flashTimer = 32;
            }
            if (p->flashTimer == 0) {
                SetState(p, 1);
                *enter = 1;
            }
            break;
        }

        case 1:
            if (StepSequence(p)) {
                Sound_PlaySong(0x58c);
                SetState(p, 2);
                p->enter = 1;
            }
            break;

        case 2:
            p->finished = 1;
            ActorStatus_Deactivate(boss, p);
            break;
    }
}
