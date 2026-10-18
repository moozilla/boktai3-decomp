#define Player_SynchronizeInteractionDisplay sub_08158650
#define Player_TestResetFlags Player_GetControlFlags
#include "global.h"
#include "player.h"
void sub_08225B74(void *);
void sub_0802E04C(void *);
u32 Player_GetControlFlags(struct Player *, u16);
void Collider_SetPosition(void *, void *, u32);
void ActorShadow_Disable(void *);
void ActorShadow_Enable(void *);
void sub_08158650(struct Player *p)
{
    sub_08225B74(&p->positionalEffectGroup);
    sub_0802E04C(&p->positionalEffectGroup);
    if (Player_GetControlFlags(p, 8))
        p->collider.flags |= 4;
    else {
        p->collider.flags &= ~4;
        Collider_SetPosition(&p->collider, &p->position, 0);
    }
    if (Player_GetControlFlags(p, 1))
        p->secondaryDrawFlags |= 1;
    else
        p->secondaryDrawFlags &= ~1;
    if (Player_GetControlFlags(p, 0x40))
        p->animation.flags &= ~1;
    else
        p->animation.flags |= 1;
    if (Player_GetControlFlags(p, 4))
        ActorShadow_Disable(p->shadow);
    else
        ActorShadow_Enable(p->shadow);
    p->position.y += p->actionCountdown;
    p->colliderPosition = p->position;
    p->colliderPosition.y += p->terrainRecoveryCounter;
}
