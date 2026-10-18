#define SceneActor_UpdateAnimation15Cycle sub_081F5CAC
#include "global.h"
typedef struct TimedSceneActor {
    u8 pad0[0xaf];
    u8 finished, padB0, enter;
    u8 padB2[0xca];
    s32 phase;
} TimedSceneActor;
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
void sub_081F5CAC(TimedSceneActor *p, s32 elapsed)
{
    if (p->enter) {
        p->enter = 0;
        SceneActors_StartUnmirroredAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 15, 1, 4);
        p->phase = 0;
        Sound_PlaySong(0x570);
    }
    switch (p->phase) {
        case 0:
            if (elapsed > 5) {
                SceneActors_StartUnmirroredAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 16, 1, 4);
                p->phase++;
            }
            break;
        case 1:
            if (elapsed > 85) {
                SceneActors_StartUnmirroredAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 15, 1, 4);
                p->phase++;
            }
            break;
        case 2:
            if (elapsed > 91) {
                p->finished = 1;
                Sound_StopSong(0x570);
            }
            break;
    }
}
