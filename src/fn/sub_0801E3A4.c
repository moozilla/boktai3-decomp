#include "global.h"
#include "player.h"
extern int gUnk_08604D54[][3];
int SceneObject_CheckActorCompatibility(void *unused, u8 *p)
{
    int i = 0, result;
    int actor = (*(u8 **)gPlayers)[0x3a4];
    int *ids;
    int *table = &gUnk_08604D54[0][0];
    {
        int offset = p[0x38] * 12;
        ids = (int *)(offset + (int)table);
    }
scan:
    if (actor != *ids)
        goto next;
    result = 0;
    goto end;
next:
    ids++;
    i++;
    if (i <= 2)
        goto scan;
    result = 1;
end:
    return result;
}
