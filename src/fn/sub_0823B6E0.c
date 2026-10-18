#define Player_CanUseAction sub_0823B6E0
#include "global.h"
#include "player.h"
u32 sub_0823B6E0(struct Player *s, s32 action)
{
    u32 result;
    if (s->actionMode <= 1 || s->actionMode == 6) {
        if (action == 0 || action == 3 || action == 6)
            goto allowed;
        if (action == 4 && s->actionFourEnabled)
            goto allowed;
    } else {
        if (action == 0 || action == 4)
            goto allowed;
    }
    goto denied;
allowed:
    result = 1;
    goto done;
denied:
    result = 0;
done:
    return result;
}
