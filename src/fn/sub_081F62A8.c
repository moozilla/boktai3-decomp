#include "global.h"
#include "ratatosk_facing.h"
struct Ratatosk081F62A8 {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x9c - 0x52];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[5];
    u8 complete;
    u8 padB0;
    u8 entering;
    u8 padB2[0x108 - 0xb2];
    void *callback;
    u8 pad10C[0x114 - 0x10c];
    s8 savedAngle;
    u8 pad115[0x32c - 0x115];
    u8 animData[0x20];
    u8 anim[0xe52 - 0x34c];
    u16 h0, h1, pad, h2;
};
struct Sub081F62A8 {
    u8 pad[9];
    u8 angle9;
};
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);

static inline void SetFacing(struct Ratatosk081F62A8 *p, int angle)
{
    RATATOSK_SET_FACING(p, angle);
}

void sub_081F62A8(struct Ratatosk081F62A8 *p)
{
    struct Ratatosk081F62A8 *p2 = p;
    struct Sub081F62A8 *m = (struct Sub081F62A8 *)((u8 *)p + 0x48);
    if (p->entering) {
        p->entering = 0;
        SceneActors_StartDirectionalAnimation(p, p->anim, p->animData, 0, 1, 1, 4);
    } else {
        SceneActors_StartDirectionalAnimation(p, p->anim, p->animData, 0, 1, 1, 1);
    }
    {
        u32 mask = 0xc;
        p2->flags = p2->flags | mask;
    }
    if ((u8)Callback_InvokeActor(p2, p2->callback) == 0) {
        s8 *sap = &p2->savedAngle;
        int d = (s8)(m->angle9 - *sap);
        int ad = d;
        if (ad < 0)
            ad = -ad;
        if (ad <= 7) {
            SetFacing(p2, *sap);
            p2->complete = 1;
        } else {
            int a;
            if (d < 0)
                a = m->angle9 + 8;
            else
                a = m->angle9 - 8;
            SetFacing(p2, a);
        }
        p->h0 = 0;
        p->h1 = 0;
        p->h2 = 0;
    }
}
