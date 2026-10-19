
#ifdef NONMATCHING
// fuzzy: 90.8%
// foundry draft (worker best 89.6%); what differs is mostly register allocation and instruction order
#define Hresvelgr_BeginRestAnimation sub_081A1244
#define gRandomIndex                 gUnk_03005308
#define gRandomValues                gUnk_0203B400
#include "global.h"
struct ActorReaction {
    u8 unknown0[0x51];
    u8 heading;
    u8 unknown52[0x4a];
    u32 flags;
    u8 unknownA0[8];
    u8 direction4, direction8;
    u8 unknownAA[7];
    u8 pending;
    u8 unknownB2[0x62];
    s32 savedHeading : 8;
    u8 unknown115[0x5f];
    u8 collider174[0x20];
    u8 collider194[0x20];
    u8 unknown1B4[0x168];
    s16 motion[4];
};
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
void Hresvelgr_SetAnimation(struct ActorReaction *, void *, void *, u32, u32, u32);
s32 sub_08249350(s32, s32);
static inline void SetHeading(struct ActorReaction *p, s32 angle)
{
    s32 shifted;
    u16 value;
    p->heading = angle;
    shifted = p->heading + 0x60;
    shifted >>= 6;
    value = 3;
    value &= shifted;
    p->direction4 = value;
    shifted = p->heading + 0x70;
    shifted >>= 5;
    value = 7;
    value &= shifted;
    p->direction8 = value;
}
void sub_081A1244(struct ActorReaction *actor)
{
    if (actor->pending) {
        actor->pending = 0;
        SetHeading(actor, actor->savedHeading);
        Hresvelgr_SetAnimation(actor, actor->collider194, actor->collider174, 26, 2, 4);
    }
    actor->motion[0] = 0;
    actor->motion[1] = 0;
    actor->motion[2] = 0;
    actor->motion[3] = 0;
    {
        u32 flags = 12;
        actor->flags |= flags;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081A1244);
#endif
