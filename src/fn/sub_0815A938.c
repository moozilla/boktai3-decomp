#define Player_GetScriptSlot Script_GetActorIndex
#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
s32 Player_TestScriptStatus(void)
{
    struct Player *p;
    s32 index = Script_GetActorIndex();
    p = gPlayers[index];
    if (p == 0 || Script_SeekToKeyword(0x73) == 0)
        goto fail;
    index = Script_GetValue();
    if (index > 1) {
    fail:
        return 0;
    }
    if (p->scriptStatus[index] == 0)
        goto fail;
    return 1;
}
