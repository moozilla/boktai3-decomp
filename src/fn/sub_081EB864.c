#include "global.h"
extern u8 *gGameState;
u8 Save_LoadSelectedSlot(void);
u8 Save_LoadAlternateSlot(void);
void GameState_BackupActiveRegions(s32);
s32 ScreenParams3F8_SelectValidDataBank(void)
{
    s32 r;
    s16 flag;
    int failure;
    if (Save_LoadSelectedSlot()) {
        *((u16 *)(gGameState + 0x86e)) = 1;
        r = 1;
    } else {
        u8 v = Save_LoadAlternateSlot();
        if (v) {
            *((u16 *)(gGameState + 0x86e)) = 1;
            r = 0;
        } else {
            *((u16 *)(gGameState + 0x86e)) = v;
            failure = -1;
            r = failure;
        }
    }
    flag = *((s16 *)(gGameState + 0x86e));
    if (flag) {
        flag = *((s16 *)(gGameState + 0x86e));
        GameState_BackupActiveRegions(flag);
    }
    return r;
}
