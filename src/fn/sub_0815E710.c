#include "global.h"
#include "player.h"
#include "contact_actor.h"
#include "engine.h"
s32 Player_IsInActionTwelveOrThirtyEight(struct Player *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
s32 PlayerContact_ApplyResponse(struct Player *, struct ContactCollider *, void *);
s32 PlayerContact_ApplySyntheticContact(struct Player *p, s32 a, s32 b, s32 direction, s32 c, s32 d)
{
    struct ContactCollider collider;
    void *target;
    s32 timer;
    if (p->kind == 1) {
        target = &p->collider;
        timer = p->positionResetValue;
        if (timer > 0)
            return 0;
        if (!Player_IsInActionTwelveOrThirtyEight(p)) {
            Collider_Init(&collider, 0, 0x2000, 0, 16, 0, 0);
            Collider_SetAttackProperties(&collider, a, b, 0, d, c);
            collider.direction = direction;
            return PlayerContact_ApplyResponse(p, &collider, target);
        }
    }
    return 0;
}
