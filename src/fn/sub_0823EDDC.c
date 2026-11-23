#include "global.h"
#include "player.h"
u32 Player_GetConfiguredActionPower(struct Player *);
void AnimData_SelectPaletteBank(u8 *, u32);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void sub_0823EDDC(struct Player *p)
{
    u32 resource;
    p->statModifierContext.contactOrigin = p->position;
    p->statModifierContext.contactOrigin.y += 230;
    p->statModifierContext.contactHeading = ((p->facing + 5) & 7) << 5;
    resource = Player_GetConfiguredActionPower(p);
    AnimData_SelectPaletteBank(p->statModifierContext.actionPalette, 76);
    Collider_SetAttackProperties(&p->statModifierContext.attackCollider, resource,
                                 p->statModifierContext.actionTimer, 0x2000, 0,
                                 p->statModifierContext.contactParameter);
    p->statModifierContext.attackCollider.direction = p->statModifierContext.contactHeading;
    p->statModifierContext.contactTimer = 0;
}
