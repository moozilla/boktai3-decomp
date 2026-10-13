#include "global.h"

extern u8 *gUnk_020000A4;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 TileAnimation_Initialize(u8 *);
void Task_Kill(u8 *);
void TileAnimation_Update(void);
void sub_08019670(void);

u8 *sub_0801973C(void)
{
    u8 *p;
    if (gUnk_020000A4 != 0)
        return gUnk_020000A4;
    p = Task_Create(0xC, 0x200);
    if (p != 0) {
        Task_SetCallbacks(p, TileAnimation_Update, sub_08019670);
        if (TileAnimation_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
