#include "global.h"
#include "ratatosk_facing.h"

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
void Ratatosk_BeginDirectionalReaction(struct Ratatosk *p)
{
    struct Ratatosk *p2 = p;
    if (p->entering) {
        int angle;
        p->entering = 0;
        if (p->locked)
            angle = p->savedAngle;
        else
            angle = p->targetAngle + 0x80;
        RATATOSK_SET_FACING(p, angle);
        SceneActors_StartDirectionalAnimation(p, p2->anim, p2->animData, 0x1d, 0, 1, 4);
        Sound_PlaySongChecked(0x55f);
    }
    p2->motion = 0;
    p2->motionFlag = 0;
    {
        u32 value;
        u32 mask = 12;
        value = p->flags | mask;
        value |= 64;
        p->flags = value;
    }
}
