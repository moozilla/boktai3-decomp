#include "global.h"
struct Counters {
    u32 f0, f4;
    u8 f8, f9, fa;
};
extern struct Counters *gUnk_030053F8;
extern u8 *gGameState;
u8 Save_ReadHeader(void);
void Memory_Clear(void *, s32);
s32 GameState_LoadSessionOrInitDefaults(void)
{
    u8 result = Save_ReadHeader();
    if (!result) {
        if (gUnk_030053F8)
            Memory_Clear(gUnk_030053F8, 40);
        {
            u32 value = 5;
            *(u16 *)(gGameState + 0x12) = value;
        }
        {
            u32 value = 230;
            gUnk_030053F8->f4 = value;
        }
        gUnk_030053F8->f8 = result;
        gUnk_030053F8->f9 = result;
        gUnk_030053F8->fa = result;
    }
    return result;
}
