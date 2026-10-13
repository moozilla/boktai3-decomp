#include "global.h"
void sub_0824923C(u8 *, u32);
void SelectionMenu_DrawStatistics(u8 *);
u32 sub_0801B2D4(u8 *p)
{
    sub_0824923C(p, *(u32 *)(p + 0xDA4));
    SelectionMenu_DrawStatistics(p);
    return 0;
}
