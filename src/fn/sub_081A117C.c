
#ifdef NONMATCHING
// fuzzy: 81.6%
// foundry draft (worker best 81.6%); what differs is mostly register allocation and instruction order
#define Hresvelgr_UpdateRandomAnimation sub_081A117C
#define gRandomIndex                    gUnk_03005308
#define gRandomValues                   gUnk_0203B400
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
static inline void RestoreHeading(struct ActorReaction *p)
{
    if (p->pending) {
        p->pending = 0;
        p->heading = p->savedHeading;
        p->direction4 = 3 & ((p->heading + 0x60) >> 6);
        p->direction8 = 7 & ((p->heading + 0x70) >> 5);
    }
}
void sub_081A117C(struct ActorReaction *actor, u32 tick)
{
    struct ActorReaction *p = actor;
    RestoreHeading(actor);
    actor->motion[0] = 0;
    actor->motion[1] = 0;
    actor->motion[2] = 0;
    actor->motion[3] = 0;
    {
        u32 flags = 12;
        actor->flags |= flags;
    }
    if (!(tick & 7)) {
        s32 anim;
        s32 random;
        gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
        random = *(u16 *)(RANDOM_TABLE_ADDR + (gUnk_03005308 << 1));
        anim = sub_08249350(random >> 3, 5) + 27;
        Hresvelgr_SetAnimation(actor, actor->collider194, actor->collider174, anim, 2, 4);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081A117C);
#endif
