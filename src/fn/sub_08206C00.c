// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"

#ifdef NONMATCHING
// fuzzy: 83.6%
// Remaining differences: facing-change branch sharing, address lifetimes, and animation-step register allocation.
#define VulcanCannon_UpdateTargeting sub_08206C00
struct VulcanCannon {
    u8 pad0[0x18];
    s32 timer;
    u8 pad1c[0x6c - 0x1c];
    union {
        struct DisplaySequenceSprite sprite;
        struct {
            u8 pad[0x1c];
            struct SceneVec position;
        } spatial;
    } display;
    u8 pad98[0x108 - 0x98];
    void (*state)(void);
    struct DisplaySequenceState sequence;
    s32 range;
    u8 pad120[0x132 - 0x120];
    s8 facing;
    u8 unknown133;
    s16 turnDelay, unknown136, shotCount, targetVisible;
    u8 pad13c[6];
    s16 aimTimer;
    u8 pad144[0x170 - 0x144];
    u32 useDelayTable;
    u8 pad174[8];
    void *resource;
    u8 pad180[0x194 - 0x180];
    s16 delayCount, delayIndex, delays[1];
};
struct SceneVec *Player_GetEffectivePosition(s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
s32 VulcanCannon_HasClearShot(struct VulcanCannon *);
s32 Math_Atan2(s32, s32);
void sub_08207350(void);
void sub_08207010(void);
static inline u32 VulcanCannon_StepIdleAnimation(struct DisplaySequenceSprite *sprite,
                                                 struct DisplaySequenceState *state)
{
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
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
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
        asm("" : "+&r"(mask) : "r"(temp));
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

s32 VulcanCannon_UpdateTargeting(struct VulcanCannon *p)
{
    struct SceneVec *player = Player_GetEffectivePosition(0);
    s32 dx = player->x - p->display.spatial.position.x;
    s32 dz = player->z - p->display.spatial.position.z;
    s32 distance = dx * dx + dz * dz;
    struct SceneVec target = *Player_GetEffectivePosition(0);
    asm("" : : "r"(&target) : "memory");
    if (p->useDelayTable) {
        if (p->timer > p->delays[p->delayIndex]) {
            if (!p->facing)
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 8);
            else
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 12);
            p->state = sub_08207350;
            p->timer = 0;
            p->delayIndex++;
            if (p->delayIndex > p->delayCount - 1)
                p->delayIndex = 0;
        }
    } else {
        if (p->timer > p->turnDelay && p->turnDelay != 0) {
            if (!p->facing)
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 8);
            else
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 12);
            p->timer = 0;
            p->state = sub_08207350;
        }
    }
    if (distance < p->range * p->range) {
        if (!p->facing) {
            if (target.z > p->display.spatial.position.z - 0x80 &&
                target.z < p->display.spatial.position.z + 0x80 &&
                target.x > p->display.spatial.position.x) {
                if (VulcanCannon_HasClearShot(p)) {
                    p->aimTimer = 0;
                    p->targetVisible = 1;
                    p->timer = 0;
                    p->shotCount = 0;
                    p->state = sub_08207010;
                } else
                    p->targetVisible = 0;
            } else
                p->targetVisible = 0;
        } else {
            if (target.x > p->display.spatial.position.x - 0x80 &&
                target.x < p->display.spatial.position.x + 0x80 &&
                target.z > p->display.spatial.position.z) {
                if (VulcanCannon_HasClearShot(p)) {
                    p->aimTimer = 0;
                    p->targetVisible = 1;
                    p->timer = 0;
                    p->shotCount = 0;
                    p->state = sub_08207010;
                } else
                    p->targetVisible = 0;
            } else
                p->targetVisible = 0;
        }
    } else
        p->targetVisible = 0;
    if (p->aimTimer == 119) {
        u8 angle;
        player = Player_GetEffectivePosition(0);
        angle = Math_Atan2(player->x - p->display.spatial.position.x,
                           player->z - p->display.spatial.position.z);
        p->timer = 0;
        p->shotCount = 0;
        if (!p->facing) {
            if ((u8)(angle - 0x21) <= 0x7e) {
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 8);
                p->timer = 0;
                p->state = sub_08207350;
            }
        } else {
            if ((u8)(angle - 0x21) > 0x7e) {
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 12);
                p->timer = 0;
                p->state = sub_08207350;
            }
        }
    }
    VulcanCannon_StepIdleAnimation(&p->display.sprite, &p->sequence);
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08206C00);
#endif
