#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
void SceneActor_UpdateAnimation15Cycle(struct Ratatosk *p, s32 elapsed)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_StartUnmirroredAnimation(p, p->animationData, p->animation, 15, 1, 4);
        p->actionCounter = 0;
        Sound_PlaySong(0x570);
    }
    switch ((s32)p->actionCounter) {
        case 0:
            if (elapsed > 5) {
                SceneActors_StartUnmirroredAnimation(p, p->animationData, p->animation, 16, 1, 4);
                p->actionCounter++;
            }
            break;
        case 1:
            if (elapsed > 85) {
                SceneActors_StartUnmirroredAnimation(p, p->animationData, p->animation, 15, 1, 4);
                p->actionCounter++;
            }
            break;
        case 2:
            if (elapsed > 91) {
                p->actionFinished = 1;
                Sound_StopSong(0x570);
            }
            break;
    }
}
