// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "engine.h"
#include "ram.h"
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
        register u32 temp asm("r2");
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
// confidence=low: circular fragment motion around the saved center, using F422 graphics
void FragmentEffect_UpdateOrbit(struct Pool *unused, struct Fragment *p)
{
    struct ContactPosition *offset;
    struct ContactPosition *position;
    struct ContactPosition *savedOffset;
    u16 *timer;
    u32 angle;
    s32 magnitude;
    if (p->entered) {
        p->timer = 0;
        p->entered = 0;
    }
    if (p->direction > 0) {
        p->angle += p->angularSpeed;
        if (p->angle > 0xFF000)
            p->angle -= 0xFF000;
    } else {
        p->angle -= p->angularSpeed;
    }
    {
        u32 rawAngle = p->angle;
        s32 radius = p->radius;
        offset = &p->offset;
        angle = rawAngle >> 12;
        magnitude = radius << 7;
    }
    {
        s32 product = magnitude * gUnk_086149C4[(angle + 64) & 255];
        s32 value = product;
        savedOffset = offset;
        {
            s32 scaled = Truncate(value);
            offset->x = scaled;
        }
    }
    offset->y = 0;
    {
        s32 product;
        angle &= 255;
        product = magnitude * gUnk_086149C4[angle];
        {
            s32 scaled = Truncate(product);
            offset->z = scaled;
        }
    }
    p->offset.y = 0;
    position = &p->position;
    {
        struct ContactPosition *center = &p->center;
        {
            register u32 x asm("r0") = (u16)center->x;
            {
                register u32 right asm("r2") = (u16)savedOffset->x;
                p->position.x = x + right;
            }
        }
        {
            register u32 y asm("r0") = (u16)center->y;
            {
                register u32 right asm("r3") = (u16)savedOffset->y;
                position->y = y + right;
            }
        }
        {
            register u32 z asm("r0") = (u16)center->z;
            {
                register u32 right asm("r7") = (u16)savedOffset->z;
                position->z = z + right;
            }
        }
    }
    {
        u16 *frame = &p->timer;
        s32 remainder = Mod(*frame, 30);
        timer = frame;
        if (!remainder) {
            u32 index = (gRandomIndex + 1) & 1023;
            gRandomIndex = index;
            {
                s32 sample = *(const u8 *)((u32)gRandomTable + index * 2);
                if (sample <= 127)
                    FragmentEffect_SpawnRandomParticle(position);
            }
        }
    }
    FragmentEffect_UpdateCollider(p);
    Step(p);
    (*timer)++;
    if (p->lifetime > 0)
        p->lifetime--;
    else {
        p->state = 3;
        p->entered = 1;
    }
}
