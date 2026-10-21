#include "global.h"

struct Ratatosk {
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
    u8 padB2;
    u8 locked;
    u8 padB4[0x108 - 0xb4];
    void *callback;
    u8 pad10C[0x32c - 0x10c];
    u8 animData[0x20];
    u8 anim[0xe6e - 0x34c];
    u16 motion;
    u8 motionFlag;
    u8 padE71;
    u8 targetAngle;
};
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySongChecked(u32);
static inline void Ratatosk_SetFacing(struct Ratatosk *p, int angle)
{
    int shifted;
    u32 value;
    p->angle = angle;
    shifted = p->angle + 0x60;
    shifted >>= 6;
    value = 3;
    value &= shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    shifted >>= 5;
    value = 7;
    value &= shifted;
    p->facing8 = value;
}
s32 Callback_InvokeActor(void *, void *);
void Ratatosk_UpdateDirectionalRecovery(struct Ratatosk *p, s32 timer)
{
    struct Ratatosk *p2 = p;
    if (p->entering) {
        int angle;
        p->entering = 0;
        angle = p->targetAngle + 0x80;
        Ratatosk_SetFacing(p, angle);
        {
            u16 mode = 0;
            SceneActors_StartDirectionalAnimation(p, p2->anim, p2->animData, 0x1d, mode, 1, 4);
        }
        Sound_PlaySongChecked(0x55f);
    }
    p2->motion = 0;
    p2->motionFlag = 0;
    {
        u32 value;
        u32 mask = 12;
        value = p->flags | mask;
        p->flags = value;
    }
    if (!(u8)Callback_InvokeActor(p, p->callback) && timer > 19)
        p->complete = 1;
}
