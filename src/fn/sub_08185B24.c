#include "armor_action.h"

void ActorController_ForceAction(void *, s32);
void Armor_UpdateAction17(struct ArmorActionController *p)
{
    switch (p->phase) {
        case 0:
            ActorController_ForceAction(p, 0x11);
            p->phase = 1;
            break;
        case 1:
            if (p->actionState == 0xff)
                p->phase = 0xff;
            break;
    }
}
