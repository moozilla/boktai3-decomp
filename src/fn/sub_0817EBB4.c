#include "global.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: summons a formation before rotating through an effect burst
struct BossActor {
    u8 pad0[0x51];
    u8 direction;
    u8 pad52[2];
    s16 position[4];
    u8 pad5C[0x40];
    u32 flags;
    u8 padA0[2];
    u8 phase, padA3;
    u32 timer;
    u8 direction4, direction8;
    u8 padAA[5];
    u8 pending, padB0, restart, finished;
    u8 padB3[0x51];
    void *callback;
    u8 pad108[8];
    void *target;
    u8 pad114[0x198];
    u8 animationState[0x20];
    u8 animation;
    u8 pad2CD[0x47];
    void *palette;
    u8 pad318[0x14];
    u8 effect[0x147];
    u8 impactCount;
    u8 pad474[0x90];
    u8 paletteTimer, pad505, paletteMode, paletteShift;
    u8 pad508[0x6F8];
    u16 paletteData[16];
};
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, s32, s32, s32, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
void BossSlot2_SpawnBobbingFormation(void *);
void ActorEffect_SpawnAtAngle(void *, void *, u32);
void sub_0806CCC8(void *, s32, s32);
u8 Callback_InvokeActor(void *, void *);
static inline void BossSlot2_SetFourDirection(struct BossActor *p, s32 direction)
{
    p->direction4 = direction;
}

static inline void BossSlot2_SetDirectionByte(struct BossActor *p, s32 angle)
{
    p->direction = angle;
}

static inline void BossSlot2_SetDirection(struct BossActor *p, s32 angle)
{
    p->direction = angle;
    {
        s32 d = (p->direction + 0x60) >> 6;
        s8 mask = 3;
        mask &= d;
        p->direction4 = mask;
    }
    {
        s32 d = (p->direction + 0x70) >> 5;
        u32 mask = 7;
        mask &= d;
        p->direction8 = mask;
    }
}

void BossSlot2_UpdateSummonSpinSequence(struct BossActor *p)
{
    struct BossActor *q = p;
    if (p->restart) {
        p->restart = 0;
        q->impactCount = 0;
        p->phase = 0;
        p->timer = 0;
    }
    {
        u32 mask = 0x400004;
        p->flags |= mask;
    }
    switch (p->phase) {
        case 0: {
            u32 zero = p->timer;
            if (!zero) {
                BossSlot2_SetDirectionByte(p, 32);
                {
                    u32 paletteMode = 3;
                    p->direction4 = ((p->direction + 0x60) >> 6) & paletteMode;
                    {
                        s32 d = (p->direction + 0x70) >> 5;
                        u32 mask = 7;
                        mask &= d;
                        p->direction8 = mask;
                    }
                    SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState,
                                                             0xF4, 1, 1, 4);
                    Sound_PlaySong(0x2E4);
                    Sound_PlaySong(0x1A8);
                    {
                        u8 paletteShift = 5;
                        q->paletteMode = paletteMode;
                        q->paletteShift = paletteShift;
                    }
                    q->paletteTimer = zero;
                    q->palette = q->paletteData;
                }
            }
            if ((++p->timer) > 8) {
                u32 phase = 1;
                u32 zero = 0;
                p->phase = phase;
                p->timer = zero;
            }
            break;
        }

        case 1:
            if (!p->timer) {
                SceneActors_StartUnmirroredAnimation(p, &q->animation, q->animationState, 0x12B, 2,
                                                     4);
                p->timer++;
            }
            if (p->finished == 1) {
                u32 phase = 2;
                u32 zero = 0;
                p->phase = phase;
                p->timer = zero;
            }
            break;

        case 2:
            if (!p->timer) {
                SceneActors_StartUnmirroredAnimation(p, &q->animation, q->animationState, 0x12D, 2,
                                                     4);
                BossSlot2_SpawnBobbingFormation(p);
            }
            if (p->timer > 30) {
                u32 phase = 3;
                u32 zero = 0;
                p->phase = phase;
                p->timer = zero;
            } else
                p->timer++;
            break;

        case 3:
            if (!p->timer)
                SceneActors_StartUnmirroredAnimation(p, &q->animation, q->animationState, 0x12F, 2,
                                                     4);
            if ((++p->timer) > 30) {
                u32 phase = 4;
                u32 zero = 0;
                p->phase = phase;
                p->timer = zero;
            }
            break;

        case 4:
            if (!p->timer) {
                BossSlot2_SetDirectionByte(p, 0);
                {
                    register u32 one asm("r4") = 1;
                    BossSlot2_SetFourDirection(p, 1);
                    {
                        s32 d = (p->direction + 0x70) >> 5;
                        u32 mask = 7;
                        mask &= d;
                        p->direction8 = mask;
                    }
                    SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState,
                                                             0x134, one, one, 4);
                }
                Sound_PlaySong(0x2E9);
            }
            if ((++p->timer) > 20) {
                u32 phase = 5;
                u32 zero = 0;
                p->phase = phase;
                p->timer = zero;
            }
            break;

        case 5: {
            register u8 spinPhase asm("r9");
            p->direction = p->timer + 15;
            {
                s32 d = (p->direction + 0x60) >> 6;
                u32 mask = 3;
                mask &= d;
                p->direction4 = mask;
            }
            {
                s32 d = (p->direction + 0x70) >> 5;
                spinPhase = 7;
                p->direction8 = d & spinPhase;
            }
            SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState, 0x13E, 1,
                                                     1, 1);
            ActorEffect_SpawnAtAngle(q->effect, p->position, p->timer);
            {
                u32 mask = 0x100000;
                p->flags |= mask;
                p->flags &= ~0x400000;
            }
            if ((p->timer += 16) > 512) {
                p->phase = spinPhase;
                p->timer = 0;
            }
            if (p->timer == 128)
                sub_0806CCC8(p->target, 180, -2);
        } break;

        case 7:
            if (!p->timer) {
                BossSlot2_SetDirectionByte(p, 0);
                {
                    register u32 one asm("r4") = 1;
                    BossSlot2_SetFourDirection(p, 1);
                    {
                        s32 d = (p->direction + 0x70) >> 5;
                        u32 mask = 7;
                        mask &= d;
                        p->direction8 = mask;
                    }
                    SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState,
                                                             0x134, one, one, 4);
                }
            }
            if (p->timer > 20) {
                u32 phase = 8;
                u32 zero = 0;
                p->phase = phase;
                p->timer = zero;
            } else
                p->timer++;
            break;

        case 8:
            if (!p->timer) {
                BossSlot2_SetDirection(p, 32);
                SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState, 0xF4,
                                                         1, 1, 1);
                p->timer++;
            }
            p->pending = 1;
            {
                u32 mask = ~4;
                p->flags &= mask;
            }
            Callback_InvokeActor(p, p->callback);
            break;
    }
}
