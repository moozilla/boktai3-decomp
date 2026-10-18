#include "global.h"
void Callback_InvokeActor(u8 *, u32);
void SelectionMenu_DrawStatistics(u8 *);
u32 SelectionMenu_Update(u8 *p)
{
    Callback_InvokeActor(p, *(u32 *)(p + 0xDA4));
    SelectionMenu_DrawStatistics(p);
    return 0;
}
