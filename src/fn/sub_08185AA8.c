#include "armor_action.h"

s32 DirectionalActor_TryBeginState(void *, s32);
void Armor_UpdateAction15(struct ArmorActionController *p)
{
    switch (p->phase) {
        case 0:
            if (DirectionalActor_TryBeginState(p, 0xf) != 0)
                p->phase = 1;
            break;
        case 1:
            if (p->actionState == 0xff)
                p->phase = 0xff;
            break;
    }
}
