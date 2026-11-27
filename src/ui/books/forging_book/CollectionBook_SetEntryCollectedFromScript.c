#include "global.h"
struct G {
    u8 pad[0x596];
    u16 f596;
};
extern struct G *gGameState;
s32 Script_SeekToKeyword(s32);
u32 Script_GetValue(void);
void CollectionBook_SetEntryCollectedFromScript(void)
{
    if (Script_SeekToKeyword(0x69)) {
        u32 n = Script_GetValue();
        gGameState->f596 |= 1 << n;
    }
}
