#include "armor_action.h"

s32 Armor_TryStartAction(void *, s32);
void Armor_UpdateAction48Completion(struct ArmorActionController *p)
{
    switch (p->phase) {
        case 0:
            if (Armor_TryStartAction(p, 0x30) != 0)
                p->phase = 1;
            break;
        case 1:
            if (p->actionState == 0xff)
                p->phase = 0xff;
            break;
    }
}
