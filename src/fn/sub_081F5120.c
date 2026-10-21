// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fuzzy: 98.9%
// foundry draft (worker best 96.7%); what differs is mostly register allocation and instruction order
struct Ratatosk {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x9c - 0x52];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[7];
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
void Ratatosk_BeginDirectionalReaction(struct Ratatosk *p)
{
    struct Ratatosk *p2 = p;
    u32 zero;
    if (p->entering) {
        int angle;
        p->entering = 0;
        if (p->locked)
            angle = p->savedAngle;
        else
            angle = p->targetAngle + 0x80;
        Ratatosk_SetFacing(p, angle);
        {
            u32 mode = 0;
            asm("" : "+r"(mode));
            SceneActors_StartDirectionalAnimation(p, p2->anim, p2->animData, 0x1d, mode, 1, 4);
        }
        Sound_PlaySongChecked(0x55f);
    }
    {
        u16 *dst = &p2->motion;
        zero = 0;
        *dst = zero;
        p2->motionFlag = zero;
    }
    {
        u32 value;
        u32 mask = 12;
        value = p->flags | mask;
        value |= 64;
        p->flags = value;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Ratatosk_BeginDirectionalReaction);
#endif
