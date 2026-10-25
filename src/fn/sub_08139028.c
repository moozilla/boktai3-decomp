// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "contact_swarm.h"
struct BallisticSpriteVector {
    s32 x, y, z, w;
};
struct BallisticSpriteCamera {
    u8 unk_00[0x44];
    s32 x;
};
struct BallisticSpriteEffect {
    void (*update)(struct BallisticSpriteEffect *);
    u8 unk_04[0x20 - 4];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState anim;
    u8 unk_5C[0x9c - 0x5c];
    struct BallisticSpriteVector position, velocity;
    u8 unk_BC[0xcc - 0xbc];
    s32 yOffset, initialVelocity;
    u8 entered, unk_D5, phase, unk_D7;
    u16 timer;
};
extern struct BallisticSpriteCamera *gUnk_020004B8;
extern const s16 gUnk_086149C4[];
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
void Sound_PlaySong(u32);
void SoundDelay_Update(struct BallisticSpriteEffect *);
void NativeParticle_CreateWithFlag(void *, s32, void *, s32, s32, s32, s32);
static inline s32 FrameDelay(s32 speed, s32 weight)
{
    return (speed * weight) >> 6;
}
static inline u32 BallisticSpriteEffect_StepAnimation(struct ContactSwarmSprite *sprite,
                                                      struct DisplaySequenceState *state)
{
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, flags, bit;
    u32 duration;
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
        u32 maskBit = 1;
        bit = maskBit;
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
        register u32 mask asm("r5");
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
                register s32 weight = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = FrameDelay(speed, weight);
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
static inline u32 RandomValue(void)
{
    return gUnk_086141BC[gUnk_03005304];
}
// Tentative: update the initial drift and particle trail of a ballistic sprite.
void BallisticSpriteEffect_UpdateLaunch(struct BallisticSpriteEffect *p)
{
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->phase = 0;
        p->timer = 0;
        p->entered = 0;
        Sound_PlaySong(0x1b4);
    }
    p->position.x += p->velocity.x + gUnk_086149C4[p->phase] * (p->timer >> 3);
    p->position.y += p->velocity.y - gUnk_086149C4[p->phase] * (p->timer >> 1);
    p->position.z += p->velocity.z;
    p->phase++;
    p->timer++;
    {
        struct BallisticSpriteVector *pos = &p->position;
        struct ContactPosition *dst = &p->sprite.position;
        u32 yOff = p->yOffset;
        dst->x = (pos->x >> 12) - gUnk_020004B8->x;
        dst->y = ((s32)(pos->z + (pos->y + yOff)) >> 12) + 0x60;
        dst->z = pos->z >> 12;
    }
    if (p->timer <= 19) {
        struct {
            u32 x : 16, y : 16, z : 16, pad : 16;
        } offset;
        gUnk_03005304 = (gUnk_03005304 + 1) & 1023;
        offset.x = (RandomValue() & 15) - 7;
        gUnk_03005304 = (gUnk_03005304 + 1) & 1023;
        offset.y = (RandomValue() & 15) - 7;
        NativeParticle_CreateWithFlag(&p->sprite.position, 0, &offset, 1, 1, 0, 0);
    }
    BallisticSpriteEffect_StepAnimation(&p->sprite, &p->anim);
    {
        register u16 *timer asm("r3") = &p->timer;
        asm("" : "+r"(timer));
        if (*timer > 150) {
            p->update = SoundDelay_Update;
            p->entered = 1;
        }
    }
}
