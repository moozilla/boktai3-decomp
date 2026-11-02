#include "armor_action.h"

s32 Armor_TryStartAction(void *, s32);
void Armor_QueueFallingParts(void *);
void Armor_UpdateAction10Sequence(struct ArmorActionController *p)
{
    u8 phase = p->phase;
    switch (phase) {
        case 0:
            if (Armor_TryStartAction(p, 10) != 0) {
                p->phase = 1;
                p->phaseTimer = phase;
            }
            break;
        case 1:
            if (p->actionState == 0xff) {
                Armor_QueueFallingParts(p->parts);
                p->phase = 2;
                p->phaseTimer = 0;
            }
            break;
        case 2:
            if (p->phaseTimer > 0x13 && Armor_TryStartAction(p, 12) != 0)
                p->phase = 3;
            break;
        case 3:
            if (p->actionState == 0xff)
                p->phase = 0xff;
            break;
    }
}
