#include "global.h"
#include "contact_actor.h"

#ifdef NONMATCHING
// fuzzy: 91.5%
// foundry draft (worker best 91.5%); what differs is mostly register allocation and instruction order
#define ActorImpact_CopyContact sub_08179C38
void sub_0821FF8C(void *);
void PlayerStats_AddContactHundredths(void *, void *, u32);
s32 Math_Atan2(s32, s32);
struct ActorImpact {
    u8 pad[0xbc];
    u32 valuebc, valuec0;
    u8 padc4[0x1e];
    u8 kind;
    u8 pade3[0x387];
    u16 value46a;
    u32 flags46c;
    u8 active, direction, value472;
    u8 pad473[0x7b];
    u16 value4ee;
};
void sub_08179C38(struct ContactCollider *a, struct ContactCollider *b, struct ActorImpact *c)
{
    u8 *d = (u8 *)c;
    u32 v;
    sub_0821FF8C(a);
    ((struct ActorImpact *)c)->value46a = b->cost;
    if (({
            u32 mask = 0x100;
            mask & a->flags;
        }))
        ((struct ActorImpact *)c)->direction = a->direction;
    else
        ((struct ActorImpact *)c)->direction = Math_Atan2(b->x - a->x, b->z - a->z);
    ((struct ActorImpact *)c)->value472 = a->responseParameter;
    v = a->statRewardFlags;
    *(u32 *)(d + 0xbc) = v;
    v = a->reactionFlags;
    *(u32 *)(d + 0xc0) = v;
    PlayerStats_AddContactHundredths(a, b, d[0xe2]);
    {
        u32 one = 1;
        if (a->reactionFlags & one)
            ((struct ActorImpact *)c)->active = one;
    }
    v = a->reactionFlags;
    ((struct ActorImpact *)c)->flags46c = v;
    ((struct ActorImpact *)c)->value4ee = a->responseValue;
    PlayerStats_AddContactHundredths(a, b, 12);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08179C38);
#endif
