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
    u8 padB4[0x114 - 0xb4];
    s8 savedAngle;
    u8 pad115[0x32c - 0x115];
    u8 animData[0x20];
    u8 anim[0xe6e - 0x34c];
    u16 motion;
    u8 motionFlag;
    u8 padE71;
    u8 targetAngle;
    u8 padE73[0xf04 - 0xe73];
    s32 duration;
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
void SceneActors_StartAnimation(void *, void *, void *, int, int, int);
void Ratatosk_UpdateTimedPose(struct Ratatosk *p, s32 timer)
{
    struct Ratatosk *p2 = p;
    if (p->entering) {
        p->entering = 0;
        if (p->locked)
            Ratatosk_SetFacing(p, p->savedAngle);
        SceneActors_StartAnimation(p, p->anim, p->animData, 0x20, 1, 4);
        p->duration = 60;
    }
    if (timer >= p2->duration)
        p->complete = 1;
}
