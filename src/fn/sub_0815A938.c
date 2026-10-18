#define Player_TestScriptStatus sub_0815A938
#define Player_GetScriptSlot Script_GetActorIndex
#define gPlayers gUnk_02000580
#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
s32 sub_0815A938(void)
{
    struct Player *p;
    s32 index = Script_GetActorIndex();
    p = gUnk_02000580[index];
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
