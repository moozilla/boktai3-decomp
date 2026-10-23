// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: provisional role inferred from state transitions and player-motion calls

#include "display_animation.h"
#include "scene_vec.h"
struct ActorStatusPart {
    u32 spriteFlags;
    u8 unknown4[0xc];
    u16 frame;
    u8 unknown12[0xa];
    struct SceneVec position;
    u8 unknown24[0x24];
    struct DisplaySequenceState sequence;
    u8 motion[0x38];
    struct SceneVec *anchor;
    u8 unknown94[8];
    struct SceneVec offset;
    u8 unknownA4[0xb8];
    u32 variant, flags, state, timer;
    u8 action, previousAction, reset, enter, finished;
    u8 unknown171[0x2b];
    u32 countdown;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
s32 Vanargand_GetComponentWithFlag10(void *);
struct SceneVec *Player_GetEffectivePosition(u32);
void Sound_PlaySong(u32);
static inline u32 StepPlayback(struct ActorStatusPart *owner, struct DisplaySequenceState *sequence,
                               struct ActorStatusPart *base, u32 one)
{
    struct DisplaySequenceState *state = sequence;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = one;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->spriteFlags |= 4;
        else
            owner->spriteFlags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->spriteFlags |= 8;
        else
            owner->spriteFlags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r6");
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
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
                    if (hold) {
                        state->elapsed = savedDuration;
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
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
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
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

static inline void SetAnimation(struct ActorStatusPart *p, u8 *boss, u32 id, u32 options)
{
    void *data = *(void **)(boss + 0x4d0);
    struct DisplaySequenceState *seq;
    if (id == 9) {
        void *resource = data;
        register u32 zero asm("r2");
        zero = 0;
        data = resource;
        options = zero;
    }
    seq = &p->sequence;
    Anim_SetSequence(seq, data, id, (u8)p->variant, options);
    StepPlayback(p, seq, p, 1);
}
static inline void SetVector(struct SceneVec *p, s32 x, s32 y, s32 z)
{
    ((volatile s16 *)p)[0] = x;
    p->y = y;
    p->z = z;
}
static inline void AddOffset(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
{
    out->x = (u16)b->x + (u16)a->x;
    out->y = (u16)b->y + (u16)a->y;
    out->z = (u16)b->z + (u16)a->z;
}
static inline void Difference(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
{
    out->x = (u16)a->x - (u16)b->x;
    out->y = (u16)a->y - (u16)b->y;
    out->z = (u16)a->z - (u16)b->z;
}
static inline u8 MotionEnded(struct ActorStatusPart *p)
{
    u8 result = 0;
    if (p->motion[0x29] == 2)
        result = 1;
    return result;
}
static inline u32 PlayerNear(struct ActorStatusPart *p)
{
    struct SceneVec *player = Player_GetEffectivePosition(0);
    register u32 scratch asm("r3");
    s32 x, dx;
    s32 dz;
    u32 result;
    s32 distance;
    /* The opaque scratch value is used only by allocation constraints. */
    asm("" : "=r"(scratch));
    x = player->x;
    asm("" : : "r"(scratch));
    dx = x - p->position.x;
    dz = player->z - p->position.z;
    distance = dx * dx + dz * dz;
    result = 0;
    if (distance <= 0x63ff)
        result = 1;
    return result;
}

// confidence=low: part approaches the player and chooses contact or timeout animation
void ActorStatus_UpdatePlayerApproach(u8 *boss, struct ActorStatusPart *part)
{
    u8 *owner = boss;
    struct ActorStatusPart *p = part;
    struct SceneVec target, delta;
    u8 *enter;
    u32 mask = 0x28;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        SetAnimation(p, owner, 11, 0);
        {
            u32 zero = 0;
            p->state = zero;
            p->timer = zero;
        }
        p->enter = 1;
    }
    switch (p->state) {
        case 0: {
            u32 entered;
            register void *motion asm("r6");
            {
                u8 *address = &p->enter;
                entered = *address;
                enter = address;
            }
            motion = p->motion;
            if (entered) {
                u32 zero = 0;
                *enter = zero;
                SetVector(&target, -256, zero, -256);
                {
                    struct SceneVec *origin = &p->offset;
                    AddOffset(&target, origin, &target);
                    MotionTrack_Initialize(motion, origin, &target, 0, 4);
                }
            }
            MotionTrack_Update(motion);
            if (MotionEnded(p)) {
                u32 next = 1;
                p->state = next;
                p->timer = 0;
                *enter = next;
            }
            break;
        }
        case 1: {
            u32 entered;
            void *motion;
            {
                u8 *address = &p->enter;
                entered = *address;
                enter = address;
            }
            motion = p->motion;
            if (entered) {
                u32 zero = 0;
                *enter = zero;
                Difference(&delta, Player_GetEffectivePosition(zero), p->anchor);
                MotionTrack_Initialize(motion, &p->offset, &delta, 0, 3);
            }
            MotionTrack_Update(motion);
            if (Vanargand_GetComponentWithFlag10(owner) >= 0) {
                goto completed;
            } else if (PlayerNear(p)) {
                {
                    u32 next = 2;
                    p->state = next;
                }
                p->timer = 0;
                *enter = 1;
            } else if (MotionEnded(p)) {
                {
                    u32 next = 3;
                    p->state = next;
                }
                p->timer = 0;
                *enter = 1;
            }
            break;
        }
        case 2:
            if (p->enter) {
                p->enter = 0;
                SetAnimation(p, owner, 12, 0);
                Sound_PlaySong(0x58a);
            }
            mask = 0x10;
            {
                register u32 offset asm("r3") = 0x160;
                register u32 *flags asm("r1");
                asm("" : "+r"(offset));
                flags = (u32 *)((u8 *)p + offset);
                *flags |= mask;
            }
            goto finished;
        case 3: {
            u32 entered;
            {
                u8 *address = &p->enter;
                entered = *address;
                enter = address;
            }
            if (entered) {
                u32 zero = 0;
                *enter = zero;
                SetAnimation(p, owner, 12, zero);
                Sound_PlaySong(0x58a);
            }
            {
                register u32 offset asm("r2") = 0x168;
                register u32 *timer asm("r1");
                asm("" : "+r"(offset));
                timer = (u32 *)((u8 *)p + offset);
                if (++*timer > 30) {
                    SetAnimation(p, owner, 9, 0);
                completed: {
                    u32 next = 4;
                    p->state = next;
                }
                    p->timer = 0;
                    *enter = 1;
                }
            }
            break;
        }
        case 4:
        finished:
            p->finished = 1;
            break;
    }
}
