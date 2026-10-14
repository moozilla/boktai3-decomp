#include "global.h"

void ModifierMenu_DrawOverview(u8 *);
void ModifierMenu_DrawPendingChoice(u8 *);

void sub_08040FC0(u8 *p)
{
    switch (*(s32 *)(p + 0x18)) {
        case 0:
        case 2:
            ModifierMenu_DrawOverview(p);
            break;
        case 1:
            ModifierMenu_DrawPendingChoice(p);
            break;
    }
}
