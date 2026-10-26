// COMPILER: old_agbcc
#include "global.h"
extern u8 *gGameState;
extern u16 gUnk_030054C0, gUnk_030054C4;
void Solar_UpdateMeasurement(void);
void Solar_InitControllerState(u8 *p)
{
    p[0x18] = 1;
    *(void (**)(void))(p + 0x2c) = Solar_UpdateMeasurement;
    {
        u32 zero = 0;
        p[0x19] = zero;
        *(u16 *)(p + 0x20) = zero;
        *(u16 *)(p + 0x22) = zero;
    }
    *(u16 *)(gGameState + 0x874) = gUnk_030054C0;
    *(u16 *)(gGameState + 0x876) = gUnk_030054C4;
}
