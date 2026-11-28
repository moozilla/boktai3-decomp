#include "global.h"
#include "player/player.h"

void Collider_Init(void *, s32, s32, s32, s32, struct SceneVec *, struct SceneVec *);
s32 Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetCallback(void *, void *, void *);
void Player_ApplyContactCosts(void);

/* confidence=low: configures the link player's attack collider for the current action. The
   attack kind and its three parameters live in the stat modifier context fields. */
void sub_0823A830(struct Player *p)
{
    struct ContactCollider *collider = &p->statModifierContext.attackCollider;
    struct SceneVec size;
    struct SceneVec offset;
    s32 power;
    if (p->actionMode == 6) {
        size.x = 0xaa;
        size.y = 0x32;
        size.z = 0xaa;
        offset.x = 0;
        offset.y = -20;
        offset.z = 0;
        Collider_Init(collider, 0, 0x2101, 0, (u16)(1 << p->movementRequestBlocked), &size,
                      &offset);
        Collider_SetAttackProperties(collider, 0x14, 0x32, 0x10, 2, 0x1e);
        Collider_SetCallback(collider, 0, p);
        return;
    }
    switch (p->statModifierContext.statBOverrideEnabled) {
        case 0:
            size.x = 0x5a;
            size.y = 0x32;
            size.z = 0x5a;
            offset.x = 0;
            offset.y = -25;
            offset.z = 0;
            power = 1;
            p->statModifierContext.actionTimer = 0x20;
            p->statModifierContext.contactParameter = 0x14;
            break;
        case 1:
            size.x = 0x3c;
            size.y = 0x64;
            size.z = 0x3c;
            offset.x = 0;
            offset.y = 0;
            offset.z = 0;
            power = 2;
            p->statModifierContext.actionTimer = 0x40;
            p->statModifierContext.contactParameter = 0x28;
            break;
        case 2:
            size.x = 0x50;
            size.y = 0x46;
            size.z = 0x50;
            offset.x = 0;
            offset.y = 0x1e;
            offset.z = 0;
            power = 4;
            p->statModifierContext.actionTimer = 0x30;
            p->statModifierContext.contactParameter = 0x28;
            break;
        case 4:
            size.x = 0x3c;
            size.y = 0x32;
            size.z = 0x3c;
            offset.x = 0;
            offset.y = -25;
            offset.z = 0;
            power = 0x10;
            p->statModifierContext.actionTimer = 0x20;
            p->statModifierContext.contactParameter = 0x1e;
            break;
        default:
            return;
    }
    Collider_Init(collider, 0, 0x2101, 0, (u16)(1 << p->movementRequestBlocked), &size, &offset);
    Collider_SetAttackProperties(collider, p->statModifierContext.actionPhase,
                                 p->statModifierContext.actionTimer, power, 0,
                                 p->statModifierContext.contactParameter);
    Collider_SetCallback(collider, Player_ApplyContactCosts, p);
}
