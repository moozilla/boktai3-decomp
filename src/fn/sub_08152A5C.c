// confidence=low: tentative cue role inferred from named callers and callback behavior.
// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
// confidence=low: motion 0x1D applies decaying recoil until standing resumes.
struct P {
    u8 pad0[0x30];
    s16 x, xf, y, yf, vx, vxf, vy, vyf;
    u8 padpos[0x14C - 0x40];
    struct ContactSwarmSprite sprite;
    u8 pad1[0x3A2 - 0x14C - sizeof(struct ContactSwarmSprite)];
    u8 facing, flip, dir;
    u8 pad2[0x3FC - 0x3A5];
    struct DisplaySequenceState seq;
    const void *resource;
    u8 pad3[0x458 - 0x410];
    u8 phase;
    u8 pad4;
    u16 timer;
    u8 pad5[0x4E8 - 0x45C];
    u16 scale;
    u8 pad6[0x58C - 0x4EA];
    void (*fn)(struct P *);
    s16 tx, txf, ty, tyf;
    u16 speed;
    u8 pad7[3];
    s8 finalDir;
};
void ActorSequence_Begin(struct P *, s32, s32);
void Player_AddControlFlags(struct P *, u32);
void PlayerCue_UpdateHidden(struct P *);
void PlayerCue_CompleteAction(struct P *);
void PlayerCue_EnterStandUpdate(struct P *);
void Player_RestartAnimationFromSetup(struct P *);
extern const s16 gUnk_086149C4[];
void Sound_PlaySong(u32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
inline static void Step_UpdateRecoilSlide(struct ContactSwarmSprite *sprite,
                                          struct DisplaySequenceState *state)
{
    const volatile u16 *command;
    u32 duration;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
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
        if (first != ((((*command) & 0x30) >> 4) & bit))
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
                command = (const volatile u16 *)(((u32)table) + offset);
            }
            state->frameWeight = (*command) & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!((u8)speed))
                    state->duration = 1;
            }
        }
    }
}

void PlayerCue_UpdateRecoilSlide(struct P *p)
{
    s32 angle;
    s32 value;
    register u32 scratch asm("r1");
    if (!p->timer) {
        Sound_PlaySong(0xE6);
        p->timer++;
    }
    Anim_SetSequence(&p->seq, p->resource, 1, p->facing, p->flip);
    Step_UpdateRecoilSlide(&p->sprite, &p->seq);
    // Keep r1 live through the speed test. The value is used only by empty constraints.
    asm("" : "=r"(scratch));
    if (p->speed)
        asm("" : : "r"(scratch));
    if (p->speed) {
        angle = ((p->dir + 5) & 7) << 5;
        value = gUnk_086149C4[(angle + 64) & 255] * p->speed;
        p->vx = -Truncate(value);
        value = gUnk_086149C4[angle] * p->speed;
        p->vy = -Truncate(value);
        p->speed = (p->speed * 3) >> 2;
    } else {
        p->speed = 0;
        ActorSequence_Begin(p, 0, 0);
        p->fn = Player_RestartAnimationFromSetup;
        PlayerCue_CompleteAction(p);
    }
}
