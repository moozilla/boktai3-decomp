// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct SceneEventActor {
    u8 unknown0[0x51];
    u8 angle;
    u8 unknown52[0x4A];
    u32 flags;
    u8 unknownA0[2];
    u8 phase, unknownA3;
    u32 timer;
    u8 facing, facing8;
    u8 unknownAA[4];
    u8 mode, finished;
    u8 unknownB0;
    u8 restart;
    u8 animationComplete;
    u8 unknownB3[0x19];
    u8 requestedAngle;
    u8 unknownCD[0x37];
    void *update;
    u8 unknown108[0x3FC];
    s8 wait;
};
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_StopSong(u32);
void SceneVolume_ResetPendingSequence(void *);
void SceneEventActor_UpdateSongStopTransition(struct SceneEventActor *actor)
{
    if (actor->restart) {
        actor->restart = 0;
        actor->phase = 0;
        actor->timer = 0;
        Sound_StopSong(0x239);
    }
    {
        u32 mask = 0x400004;
        actor->flags |= mask;
    }
    switch (actor->phase) {
        case 0:
            if (++actor->timer > 8 && !actor->wait) {
                SceneVolume_ResetPendingSequence(actor);
                actor->phase = 1;
                actor->timer = 0;
            }
            break;
        case 2:
        default:
            break;
        case 1:
            if (!actor->timer) {
                SceneActors_StartUnmirroredAnimation(actor, (u8 *)actor + 0x2CC,
                                                     (u8 *)actor + 0x2AC, 0x12F, 2, 4);
                actor->timer++;
            }
            if (actor->animationComplete) {
                u32 zero;
                u32 phase;
                int shifted;
                u32 facing;
                facing = 0x20;
                actor->angle = facing;
                zero = 0;
                phase = 2;
                asm("" : "+r"(phase));
                shifted = actor->angle + 0x60;
                shifted >>= 6;
                facing = 3;
                facing &= shifted;
                actor->facing = facing;
                {
                    register int sh asm("r0");
                    register u32 m asm("r1");
                    sh = actor->angle + 0x70;
                    asm("" : "+r"(sh));
                    sh >>= 5;
                    m = 7;
                    asm("" : "+r"(m));
                    m &= sh;
                    actor->facing8 = m;
                }
                SceneActors_StartEightDirectionAnimation(actor, (u8 *)actor + 0x2CC,
                                                         (u8 *)actor + 0x2AC, 0xF4, 1, 1, 4);
                actor->phase = phase;
                actor->timer = zero;
                actor->finished = 1;
            }
            break;
    }
}
