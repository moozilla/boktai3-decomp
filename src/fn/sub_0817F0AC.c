#include "global.h"
// confidence=low: plays a charge animation sequence and starts a converging particle pulse
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
static inline void SetDirection(struct BossActor *p, s32 angle)
{
    p->direction = angle;
    {
        s32 d = (p->direction + 0x60) >> 6;
        u32 mask = 3;
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
static inline void SetPalette(struct BossActor *p, u32 mode, u32 shift, u32 zero)
{
    p->paletteMode = mode;
    p->paletteShift = shift;
    p->paletteTimer = zero;
    p->palette = p->paletteData;
}
void ActorPosition_SetOffsetPoint(void *);
void SceneActors_SpawnConvergingParticle(void *, void *);
void BossSlot2_UpdateChargePaletteSequence(struct BossActor *p)
{
    if (p->restart) {
        p->restart = 0;
        SetDirection(p, 32);
        SceneActors_StartEightDirectionAnimation(p, &p->animation, p->animationState, 0xF4, 1, 1,
                                                 1);
        ActorPosition_SetOffsetPoint(p);
        p->phase = 0;
        p->timer = 0;
    }
    {
        u32 mask = 0x400004;
        p->flags |= mask;
    }
    switch (p->phase) {
        case 0:
            if (!p->timer) {
                SceneActors_StartUnmirroredAnimation(p, &p->animation, p->animationState, 0x12B, 2,
                                                     4);
                p->timer++;
            }
            if (p->finished == 1) {
                u32 phase = 1, zero = 0;
                p->phase = phase;
                p->timer = zero;
            }
            break;
        case 1:
            if (!p->timer)
                SceneActors_StartUnmirroredAnimation(p, &p->animation, p->animationState, 0x12D, 2,
                                                     4);
            if (p->timer > 30) {
                u32 phase = 2, zero = 0;
                p->phase = phase;
                p->timer = zero;
            } else
                p->timer++;
            break;
        case 2:
            if (!p->timer) {
                SetPalette(p, 4, 5, p->timer);
                Sound_PlaySong(0x239);
                p->finished = 1;
                p->timer++;
            }
            SceneActors_SpawnConvergingParticle(p, (u8 *)p + 0x5F8);
            break;
    }
}
