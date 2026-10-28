#include "global.h"
#include "runtime.h"
struct Counters {
    u32 f0, f4;
    u8 f8, f9, fa;
};
extern u8 *gGameState;
u8 Save_ReadHeader(void);
void Memory_Clear(void *, s32);
s32 GameState_LoadSessionOrInitDefaults(void)
{
    u8 result = Save_ReadHeader();
    if (!result) {
        if (((struct Counters *)gSessionState))
            Memory_Clear(((struct Counters *)gSessionState), 40);
        {
            u32 value = 5;
            *(u16 *)(gGameState + 0x12) = value;
        }
        {
            u32 value = 230;
            ((struct Counters *)gSessionState)->f4 = value;
        }
        ((struct Counters *)gSessionState)->f8 = result;
        ((struct Counters *)gSessionState)->f9 = result;
        ((struct Counters *)gSessionState)->fa = result;
    }
    return result;
}
