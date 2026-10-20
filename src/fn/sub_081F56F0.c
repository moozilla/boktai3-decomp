#define Ratatosk_UpdateLongTimedPose sub_081F56F0
#include "global.h"
struct Ratatosk081F56F0 {
    u8 pad0[0xAF];
    u8 complete;
    u8 padB0;
    u8 entering;
    u8 padB2[0x32c - 0xb2];
    u8 animData[0x20];
    u8 anim[0xf04 - 0x34c];
    s32 duration;
};
void SceneActors_StartAnimation(void *, void *, void *, int, int, int);
void Ratatosk_UpdateLongTimedPose(struct Ratatosk081F56F0 *p, s32 timer)
{
    struct Ratatosk081F56F0 *p2 = p;
    if (p->entering) {
        p->entering = 0;
        SceneActors_StartAnimation(p, p->anim, p->animData, 0x21, 2, 4);
        p->duration = 120;
    }
    if (timer >= p2->duration)
        p->complete = 1;
}
