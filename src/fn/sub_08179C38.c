// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "contact_actor.h"

void Collision_CalculateDamageResponse(void *);
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
void ActorImpact_CopyContact(struct ContactCollider *a, struct ContactCollider *b,
                             struct ActorImpact *c)
{
    u8 *d = (u8 *)c;
    u8 *q;
    u32 v;
    int mask;
    Collision_CalculateDamageResponse(a);
    ((struct ActorImpact *)c)->value46a = b->cost;
    mask = 0x100;
    if (a->flags & mask)
        ((struct ActorImpact *)c)->direction = a->direction;
    else
        ((struct ActorImpact *)c)->direction = Math_Atan2(b->x - a->x, b->z - a->z);
    ((struct ActorImpact *)c)->value472 = a->responseParameter;
    v = a->statRewardFlags;
    q = d + 0xbc;
    *(u32 *)q = v;
    asm("" : "+r"(q));
    v = a->reactionFlags;
    q += 4;
    *(u32 *)q = v;
    q += 0x22;
    PlayerStats_AddContactHundredths(a, b, *q);
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
