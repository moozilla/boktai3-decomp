#include "global.h"
#include "engine.h"
void Solar_SaveExposureTimestamp(void);
u32 Solar_HasExposureCooldownElapsed(void);
#define GS(type, offset) (*(type *)((u8 *)gGameState + (offset)))

static inline u8 Solar_IsBlocked(void)
{
    u32 m = 0x8400;
    if (GS(u32, 0x868) & m)
        return 1;
    return 0;
}

void sub_0822C4D8(void)
{
    if (GS(s32, 0x504) > 29999) {
        if (GS(s16, 0x876) > 2 && !Solar_IsBlocked()) {
            Solar_SaveExposureTimestamp();
            if ((GS(s16, 0x502) += GS(u16, 0x876)) > 5000)
                GS(s16, 0x502) = 5000;
        } else {
            if (GS(s16, 0x502) > 0)
                GS(s16, 0x502)--;
            if (Solar_HasExposureCooldownElapsed()) {
                GS(s32, 0x504) = 0;
                GS(s16, 0x502) = 0;
            }
        }
    }
}
