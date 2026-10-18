#include "global.h"
/* TextWindow_GetNumberWidth: Returns the decimal display width of an indexed number including its sign */
#define TextWindow_GetNumberWidth sub_08033AB0

u8 *TextWindow_Find(u32);
s32 TextState_GetNumberWidth(void *, u32);

s32 sub_08033AB0(u32 id, u32 arg)
{
    u8 *w = TextWindow_Find(id);
    if (w == NULL)
        return -1;
    return TextState_GetNumberWidth(w + 0x60, arg);
}
