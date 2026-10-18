#define Player_IsActionAllowed sub_08141800
#include "global.h"
#include "player.h"
u32 sub_08141800(struct Player *p, u32 action)
{
    if (p->actionMode <= 1 || p->actionMode == 6) {
        if (action == 0 || action == 3 || action == 19 || action == 20 || action == 6)
            goto allowed;
        if (action == 4 && p->actionFourEnabled) goto allowed;
    } else if (p->actionMode == 5) {
        if (action == 0) goto allowed;
    } else {
        if (action == 0 || action == 4) goto allowed;
    }
    goto denied;
allowed:
    return 1;
denied:
    return 0;
}
