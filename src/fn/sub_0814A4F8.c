#include "global.h"
#include "player.h"

void sub_0814A4F8(struct Player *p)
{
    u8 index = p->statModifierContext.savedActionIndex;
    if (p->statModifierContext.savedActionIndex > 0)
        p->actionVariant = index + 1;
    else
        p->actionVariant = 0;
}
