#include "global.h"
#include "player.h"
#define Player_InitializeActionContact sub_0814D724
#define Player_CollectActionFlags      sub_0814A854
s32 Player_GetAttackPower(struct Player *);
u32 sub_0814A854(struct Player *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void sub_0814D724(struct Player *p)
{
    s32 kind, flags, parameter;
    p->statModifierContext.contactOrigin = p->position;
    p->statModifierContext.contactOrigin.y += 200;
    p->statModifierContext.contactHeading = ((p->facing + 5) & 7) << 5;
    kind = Player_GetAttackPower(p);
    flags = sub_0814A854(p);
    parameter = p->statModifierContext.contactParameter;
    p->statModifierContext.contactExtentA = 8;
    p->statModifierContext.contactExtentB = 8;
    p->statModifierContext.contactCounter = 0;
    Collider_SetAttackProperties(&p->statModifierContext.drain, kind, p->statModifierContext.actionTimer, 1, flags,
                 parameter);
    p->statModifierContext.contactDirection = p->statModifierContext.contactHeading;
    p->statModifierContext.contactTimer = 0;
    p->contactCounted = 0;
}
