#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 TileAnimation_Initialize(u8 *);
void Task_Kill(u8 *);
void TileAnimation_Update(void);
void TileAnimation_Destroy(void);

u8 *TileAnimation_Create(void)
{
    u8 *p;
    if (gTileAnimation != 0)
        return gTileAnimation;
    p = Task_Create(0xC, 0x200);
    if (p != 0) {
        Task_SetCallbacks(p, TileAnimation_Update, TileAnimation_Destroy);
        if (TileAnimation_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
