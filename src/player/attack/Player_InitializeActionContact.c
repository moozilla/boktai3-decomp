#include "global.h"
#include "player/player.h"
s32 Player_GetAttackPower(struct Player *);
u32 ActorFlags_GetResponseBits(struct Player *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Player_InitializeActionContact(struct Player *p)
{
    s32 kind, flags, parameter;
    p->statModifierContext.contactOrigin = p->position;
    p->statModifierContext.contactOrigin.y += 200;
    p->statModifierContext.contactHeading = ((p->facing + 5) & 7) << 5;
    kind = Player_GetAttackPower(p);
    flags = ActorFlags_GetResponseBits(p);
    parameter = p->statModifierContext.contactParameter;
    p->statModifierContext.motionSpeed = 8;
    p->statModifierContext.swingFrames = 8;
    p->statModifierContext.windupFrames = 0;
    Collider_SetAttackProperties(&p->statModifierContext.attackCollider, kind,
                                 p->statModifierContext.actionTimer, 1, flags, parameter);
    p->statModifierContext.attackCollider.direction = p->statModifierContext.contactHeading;
    p->statModifierContext.contactTimer = 0;
    p->contactCounted = 0;
}
