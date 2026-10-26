#include "global.h"
u32 Script_SeekToKeyword(u32);
u32 Script_GetValue(void);
extern u8 *gGameState;
void Collection_UnlockEntryFromScript(void)
{
    if (Script_SeekToKeyword(0x69)) {
        u32 v = Script_GetValue();
        u32 *q = (u32 *)(gGameState + 0x728);
        *q = *q | (1 << v);
    }
}
