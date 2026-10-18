#define Player_SaveActionIndex sub_0823D714
#define Player_GetActionIndex sub_0823AB5C
#include "global.h"
#include "player.h"

s32 sub_0823AB5C(struct Player *);

void sub_0823D714(struct Player *p)
{
    s32 index = sub_0823AB5C(p);
    p->statModifierContext.savedActionIndex = index;
    if ((s8)index >= 0)
        p->actionVariant = index + 1;
    else
        p->actionVariant = 0;
}
