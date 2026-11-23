#include "global.h"
#include "ratatosk.h"
#include "ratatosk_facing.h"
struct Sub081F62A8 {
    u8 pad[9];
    u8 angle9;
};
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);
static inline void SetFacing(struct Ratatosk *p, int angle)
{
    RATATOSK_SET_FACING(p, angle);
}

void sub_081F62A8(struct Ratatosk *p)
{
    struct Ratatosk *p2 = p;
    struct Sub081F62A8 *m = (struct Sub081F62A8 *)((u8 *)p + 0x48);
    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0, 1, 1, 4);
    } else {
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0, 1, 1, 1);
    }
    {
        u32 mask = 0xc;
        p2->flags = p2->flags | mask;
    }
    if ((u8)Callback_InvokeActor(p2, p2->actionReset) == 0) {
        s8 *sap = &p2->savedAngle;
        int d = (s8)(m->angle9 - *sap);
        int ad = d;
        if (ad < 0)
            ad = -ad;
        if (ad <= 7) {
            SetFacing(p2, *sap);
            p2->actionFinished = 1;
        } else {
            int a;
            if (d < 0)
                a = m->angle9 + 8;
            else
                a = m->angle9 - 8;
            SetFacing(p2, a);
        }
        p->scaleX = 0;
        p->scaleZ = 0;
        p->scaleMode = 0;
    }
}
