#include "global.h"
#include "player/player.h"

s32 Div(s32, s32);

s32 sub_0823D750(struct Player *p)
{
    s32 power;

    switch (p->statModifierContext.statBOverrideEnabled) {
        case 0:
        case 1:
        case 2: {
            u32 base = (p->derivedStats[2] + 99) >> 1;
            s32 bonus = p->statModifierContext.actionDuration + p->statModifierContext.powerBonus;
            if (bonus < 0)
                bonus = 0;
            return base + bonus;
        }
        case 3:
            power = (p->derivedStats[2] >> 1) + 99;
            return power >> 1;
        case 4:
            power = p->statModifierContext.actionDuration + (Div(40, 10) - 8) * 5;
            power <<= 1;
            return power;
    }
}
