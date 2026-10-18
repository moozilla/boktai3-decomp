#include "global.h"

extern u8 *gUnk_020001B8;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 Pool1B8_Init(u8 *);
void Task_Kill(u8 *);
void TileTrigger_TickManager(void);
void TilePool_Destroy(void);

u8 *Pool1B8_Create(void)
{
    u8 *p;
    if (gUnk_020001B8 != 0)
        return gUnk_020001B8;
    p = Task_Create(0x8, 0x6c);
    if (p != 0) {
        Task_SetCallbacks(p, TileTrigger_TickManager, TilePool_Destroy);
        if (Pool1B8_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
