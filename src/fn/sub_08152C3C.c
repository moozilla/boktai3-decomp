// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
struct CueActor {
    u8 pad0[0x1D0];
    u32 f1D0;
    u8 pad1D4[0x18];
    u16 f1EC;
    u16 f1EE;
    u16 f1F0;
    s16 f1F2;
    u8 pad1F4[0x2E];
    u16 f222;
    u8 pad224[0x4];
    u32 f228;
    u8 pad22C[0x176];
    u8 f3A2;
    u8 f3A3;
    u8 f3A4;
    u8 pad3A5[0x73];
    u8 f418;
    u8 pad419[0x17];
    u16 f430;
    u8 pad432[0x26];
    u8 f458;
    u8 pad459[0x1];
    u16 f45A;
    u8 pad45C[0x8];
    u8 f464;
    u8 f465;
    u8 pad466[0x80];
    u16 f4E6;
    u8 pad4E8[0x2];
    u16 f4EA;
    u8 pad4EC[0x2];
    u8 f4EE;
    u8 pad4EF[0x47];
    u16 f536;
    u16 f538;
    u8 pad53A[0x69];
    u8 f5A3;
    u8 pad5A4[0x158];
    u16 f6FC;
    u8 pad6FE[0x3C6];
    u16 fAC4;
    u8 padAC6[0x10];
    u16 fAD6;
    u16 fAD8;
};
// Remaining: phase layout, field addressing, and playback register allocation.
#define U8(o)  (*(u8 *)(p + (o)))
#define U16(o) (*(u16 *)(p + (o)))
#define S16(o) (*(s16 *)(p + (o)))
#define U32(o) (*(u32 *)(p + (o)))

s32 Player_ConsumeStateEntry(void *);
void Player_SetActionPhase(void *, s32);
u32 Player_SetAnimation(void *, u32, u32, u32);
s32 ActorVariant_GetTableValue(s32, s32);
void Player_UpdateFacingSide(void *);
void Player_InitVisualResources(void *);
void ActorSequence_Begin(void *, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void SpiralTrail_CreateWithParticles(void *, s32, void *, s32, s32);
void Sound_PlaySong(u32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
static inline u32 StepEnd(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st, u32 phase)
{
    struct ContactSwarmSprite *sprite = sp;
    struct DisplaySequenceState *state = st;
    const volatile u16 *command;
    u32 duration, flags, bit;
    u32 ended;
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
        u32 maskBit = phase;
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
                        goto ended_animation;
                    } else {
                        state->index = state->frameCount - 1;
                        goto ended_animation;
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
                    goto ended_animation;
                } else {
                }
            }
            ended = 0;
            goto update_frame;
        ended_animation:
            ended = 1;
        update_frame: {
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
// confidence=low: reduces departure scale and returns to standing after animation.
// confidence=low: phased cue alternates facing and restores standing motion.
inline static u32 StepEnd_08152C3C(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st,
                                   u32 phase)
{
    struct ContactSwarmSprite *sprite = sp;
    struct DisplaySequenceState *state = st;
    const volatile u16 *command;
    u32 duration;
    u32 flags;
    u32 bit;
    u32 ended;
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
        u32 maskBit = phase;
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
                s16 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                        goto ended_animation;
                    } else {
                        state->index = state->frameCount - 1;
                        goto ended_animation;
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
                    goto ended_animation;
                } else {
                }
            }
            ended = 0;
            goto update_frame;
        ended_animation:
            ended = 1;

        update_frame: {
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
        } else {
            ended = 0;
        }
    }
    return ended;
}

void sub_08152C3C(struct CueActor *p)
{
    if (Player_ConsumeStateEntry(p)) {
        p->f430 = 4000;
        p->fAD6 = 0x16E;
        p->fAD8 = 0;
        p->f4EE = 1;
        Sound_PlaySong(0x306);
        p->f1D0 &= ~1;
        p->f1EC = (*((u16 *)(((u8 *)gUnk_02000710) + 0x30))) - 6;
        p->f1EE = 450 + (*((u16 *)(((u8 *)gUnk_02000710) + 0x32)));
        p->f1F0 = (*((u16 *)(((u8 *)gUnk_02000710) + 0x34))) + 6;
        Anim_SetSequence(((u8 *)p) + 0x218, *((void **)(((u8 *)p) + 0x22C)), 21, 0, 0);
        p->f228 = 0;
    }
    if (p->f228)
        Anim_SetSequence(((u8 *)p) + 0x218, *((void **)(((u8 *)p) + 0x22C)), 21, 0, 0);
    p->f228 = StepEnd_08152C3C((void *)(((u8 *)p) + 0x1D0), (void *)(((u8 *)p) + 0x218), 1);
    if (p->f222 == 21) {
        if (p->f1F2 == 1)
            p->f1EE -= 10;
        else
            p->f1EE += 10;
    }
    if ((p->f1F2 == 1) && (((s16)p->f1EE) < ((*((s16 *)(((u8 *)gUnk_02000710) + 0x32))) + 220)))
        p->f1EE = (*((u16 *)(((u8 *)gUnk_02000710) + 0x32))) + 220;
    switch (p->f458) {
        case 0:
            if (((u8)(p->f3A4 - 2)) <= 4) {
                p->f3A4 = 4;
                p->f3A2 = 4;
                p->f3A3 = 0;
            } else {
                p->f3A4 = 0;
                p->f3A2 = 0;
                p->f3A3 = 0;
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->f418, 0), 64, 1);
            p->f3A2 >>= 2;
            if (p->f4EE) {
                struct {
                    u32 a;
                    u32 b;
                } particles;
                particles.a = 0;
                particles.b = 15;
                SpiralTrail_CreateWithParticles(p, 0, &particles, 50, 50);
                Sound_PlaySong(0x39B);
            }
            Player_SetActionPhase(p, 1);
            break;

        case 1:
            if (p->f4EE) {
                p->fAD8 += 3;
                if (p->fAD8 > 32)
                    p->fAD8 = 32;
            }
            p->f6FC = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->f418, 8), 64, 1)) {
                if (p->f4EE)
                    Player_SetActionPhase(p, 2);
                else
                    Player_SetActionPhase(p, 3);
            }
            break;

        case 2: {
            u32 timerMask;
            if ((p->f45A & 7) < (7 - (p->f45A >> 2)))
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->f418, 10), 64, 1);
            else if (p->f418 == 0)
                Player_SetAnimation(p, ActorVariant_GetTableValue(2, 10), 64, 1);
            else
                Player_SetAnimation(p, ActorVariant_GetTableValue(0, 10), 64, 1);
            p->fAD8 += 3;
            timerMask = 0xFFFF;
            asm("" : "+r"(timerMask));
            if (p->fAD8 > 32)
                p->fAD8 = 32;
            p->f6FC = 30;
            if (((++p->f45A) & timerMask) > 9) {
                p->f418 = (p->f418) ? (0) : (2);
                Player_InitVisualResources(p);
                p->fAC4 = 0xFFFF;
                if ((p->f536 != 21) && (p->f536 != 49))
                    p->f538 = 0;
                Player_SetActionPhase(p, 4);
                p->f1D0 |= 1;
            }
            break;
        }

        case 3:
            if (!p->f45A)
                Sound_PlaySong(0x192);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->f418, 10), 64, 1);
            p->f6FC = 30;
            if ((++p->f45A) > 15)
                Player_SetActionPhase(p, 4);
            break;

        case 4:
            p->f6FC = 15;
            ++p->f45A;
            if (p->f418 != 2) {
                if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->f418, 11), 64, 1)) {
                    Player_UpdateFacingSide(p);
                    ActorSequence_Begin(p, 0, 0);
                    break;
                }
            } else {
                p->f4E6 = 0xFFF4;
                p->f4EA = 12;
                Anim_SetKeyframes(((u8 *)p) + 0x8C, ((u8 *)p) + 0x6C, 0x1AB, 1, 0);
                Anim_Step(((u8 *)p) + 0x8C, ((u8 *)p) + 0x6C);
            }
            Player_InitVisualResources(p);
            p->fAC4 = 0xFFFF;
            p->fAD8 = 0;
            if ((++p->f45A) > 29) {
                Player_UpdateFacingSide(p);
                p->f464 = 1;
                p->f465 = 2;
                Player_UpdateFacingSide(p);
                PlayerCue_EnterStandUpdate(p);
                p->f5A3 = 1;
            }
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08152C3C);
#endif
