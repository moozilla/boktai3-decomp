#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySong(u32);
void SceneActor_UpdateAnimation7Timer(struct Ratatosk *p, s32 elapsed)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (p->locked)
            Ratatosk_SetFacing(p, p->savedAngle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 7, 2, 3, 4);
        Sound_PlaySong(0x565);
    }
    if (elapsed > 56)
        p->actionFinished = 1;
}
