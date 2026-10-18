#include "global.h"
#include "player.h"

s32 Player_GetActionIndex(struct Player *);

void Player_SaveActionIndex(struct Player *p)
{
    s32 index = Player_GetActionIndex(p);
    p->statModifierContext.savedActionIndex = index;
    if ((s8)index >= 0)
        p->actionVariant = index + 1;
    else
        p->actionVariant = 0;
}
