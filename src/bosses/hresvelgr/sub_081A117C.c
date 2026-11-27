#include "global.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"

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
void Hresvelgr_SetAnimation(struct ActorReaction *, void *, void *, u32, u32, u32);
s32 sub_08249350(s32, s32);
static inline void RestoreHeading(struct ActorReaction *p)
{
    if (p->pending) {
        p->pending = 0;
        p->heading = p->savedHeading;
        {
            u32 v;
            s32 shifted = (p->heading + 0x60) >> 6;
            v = 3;
            v &= shifted;
            p->direction4 = v;
            shifted = (p->heading + 0x70) >> 5;
            v = 7;
            v &= shifted;
            p->direction8 = v;
        }
    }
}
void Hresvelgr_UpdateRandomAnimation(struct ActorReaction *actor, u32 tick)
{
    struct ActorReaction *p = actor;
    struct ActorReaction **actorRef;
    u32 t = tick;
    RestoreHeading(p);
    p->motion[0] = 0;
    actorRef = &p;
    (*actorRef)->motion[1] = 0;
    (*actorRef)->motion[2] = 0;
    (*actorRef)->motion[3] = 0;
    {
        u32 flags = 12;
        p->flags |= flags;
    }
    if (!(t & 7)) {
        s32 anim;
        s32 random;
        gRandomIndex = (gRandomIndex + 1) & 0x3ff;
        random = *(u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
        anim = sub_08249350(random >> 3, 5) + 27;
        Hresvelgr_SetAnimation(*actorRef, (*actorRef)->collider194, (*actorRef)->collider174, anim,
                               2, 4);
    }
}
