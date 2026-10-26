// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "engine.h"
struct Fragment {
    u32 flags;
    u8 active, unknown5[3];
    s8 scaleX, scaleY;
    u8 unknownA[2];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct ContactPosition position;
    u8 unknown24[8];
    struct DisplaySequenceState sequence;
    u8 collider[0x54];
    struct ContactPosition center, offset;
    u32 angle, angularSpeed;
    s16 direction;
    u16 unknownAA;
    s16 radius, scaleTimer;
    u32 unknownB0;
    s32 lifetime;
    u16 unknownB8;
    u8 state;
    s8 slot;
    u16 timer;
    u8 entered;
};
struct Pool {
    u8 pad[0x38];
    u32 used;
};
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
s32 Mod(s32, s32);
s32 Div(s32, s32);
void FragmentEffect_SpawnRandomParticle(struct ContactPosition *);
void FragmentEffect_UpdateCollider(struct Fragment *);
void Sprite_Remove(void *);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline void Step(struct Fragment *sprite)
{
    struct DisplaySequenceState *state = &sprite->sequence;
    const volatile u16 *command;
    u32 duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
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
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
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
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                } else {
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
        }
    }
}
// confidence=low: F422 fragment state interpolates sprite scale on appearance
void FragmentEffect_GrowToFullSize(struct Pool *pool, struct Fragment *p)
{
    s16 *scaleTimer;
    u16 *timer;
    if (p->entered) {
        p->flags = (p->flags & ~1) | 2;
        p->scaleTimer = 30;
        p->timer = 0;
        p->entered = 0;
        p->scaleX = 1;
        p->scaleY = 1;
    }
    {
        u16 *frame = &p->timer;
        s32 remainder = Mod(*frame, 12);
        timer = frame;
        if (!remainder)
            FragmentEffect_SpawnRandomParticle(&p->position);
    }
    {
        s16 *initial = &p->scaleTimer;
        register s32 remaining asm("r1") = *initial;
        scaleTimer = initial;
        asm("" : "+r"(scaleTimer));
        if (remaining > 0) {
            p->scaleX += Div(64 - p->scaleX, *scaleTimer);
            p->scaleY += Div(64 - p->scaleY, *scaleTimer);
            (*scaleTimer)--;
        }
    }
    Step(p);
    {
        s32 remaining;
        register u32 hold asm("r1");
        remaining = *scaleTimer;
        asm("" : : "r"(hold));
        if (remaining <= 0) {
            p->state = 2;
            p->flags &= ~2;
            p->entered = 1;
        } else {
            u16 *frame = timer;
            (*frame)++;
        }
    }
}
