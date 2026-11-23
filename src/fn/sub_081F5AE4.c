#include "global.h"
#include "ratatosk.h"
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySong(u32);
static inline void setFacing(struct Ratatosk *p, int angle)
{
    int shifted;
    u32 value;
    p->body.direction = angle;
    shifted = p->body.direction + 0x60;
    shifted >>= 6;
    value = 3;
    value &= shifted;
    p->facing = value;
    shifted = p->body.direction + 0x70;
    shifted >>= 5;
    value = 7;
    value &= shifted;
    p->facing8 = value;
}
void SceneActor_UpdateAnimation7Timer(struct Ratatosk *p, s32 elapsed)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (p->locked)
            setFacing(p, p->savedAngle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 7, 2, 3, 4);
        Sound_PlaySong(0x565);
    }
    if (elapsed > 56)
        p->actionFinished = 1;
}
