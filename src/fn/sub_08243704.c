#include "global.h"
#include "player.h"
s32 Script_GetOptionalIValue(void);
bool32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);

void sub_08243704(void)
{
    s32 i = Script_GetOptionalIValue();
    if (gPlayers[i] != NULL) {
        if (Script_SeekToKeyword('T'))
            Script_GetValue();
    }
}
