#define Player_SynchronizeColliderState sub_08242DCC
#include "global.h"
#include "player.h"
void sub_08225B74(void *);
u32 Player_GetSecondaryActionFlags(struct Player *, u16);
void Collider_SetPosition(void *, void *, u32);
void Player_SynchronizeColliderState(struct Player *p)
{
    sub_08225B74(&p->positionalEffectGroup);
    if (Player_GetSecondaryActionFlags(p, 8)) {
        p->collider.flags |= 4;
    } else {
        p->collider.flags &= ~4;
        Collider_SetPosition(&p->collider, &p->position, 0);
    }
    if (Player_GetSecondaryActionFlags(p, 1)) {
        p->secondaryDrawFlags |= 1;
    } else {
        p->secondaryDrawFlags &= ~1;
    }
    if (Player_GetSecondaryActionFlags(p, 0x40)) {
        p->animation.flags &= ~1;
    } else {
        p->animation.flags |= 1;
    }
    p->position.y += p->actionCountdown;
    p->colliderPosition = p->position;
    p->colliderPosition.y += p->terrainRecoveryCounter;
}
