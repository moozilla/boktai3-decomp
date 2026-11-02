#include "global.h"
#include "player.h"
u32 Player_GetSecondaryActionFlags(struct Player *, u16);
void CellObject_Init(void *, void *);
void Obj_StepPosition(void *);
void Collider_SetPosition(void *, void *, s32);
void ActorShadow_Disable(void *);
void ActorShadow_Enable(void *);

void sub_082424D0(struct Player *p)
{
    if (p->tetherStatusFlags == 0) {
        if (Player_GetSecondaryActionFlags(p, 16))
            p->followTarget = 0;
        else if (!p->followTarget) {
            void *position = &p->movementContactFlags;
            CellObject_Init(position, &p->position);
            p->followTarget = position;
        }
    }
    Obj_StepPosition(&p->positionalEffectGroup);
    if (Player_GetSecondaryActionFlags(p, 8))
        p->collider.flags |= 4;
    else {
        p->collider.flags &= ~4;
        Collider_SetPosition(&p->collider, &p->position, 0);
    }
    if (Player_GetSecondaryActionFlags(p, 1))
        p->animation.flags |= 1;
    else
        p->animation.flags &= ~1;
    if (Player_GetSecondaryActionFlags(p, 2))
        p->animation.flags |= 0x200;
    else
        p->animation.flags &= ~0x200;
    if (Player_GetSecondaryActionFlags(p, 64))
        p->secondaryDrawFlags &= ~1;
    else
        p->secondaryDrawFlags |= 1;
    if (Player_GetSecondaryActionFlags(p, 4))
        ActorShadow_Disable(p->shadow);
    else
        ActorShadow_Enable(p->shadow);
    p->position.y += p->actionCountdown;
    p->previousPosition = p->position;
    p->previousPosition.y += p->terrainRecoveryCounter;
}
