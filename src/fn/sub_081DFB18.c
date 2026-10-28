#include "global.h"
#include "menu_parameters.h"
extern u8 gUnk_0824FD3C[];
void memcpy(void *, const void *, u32);
s32 Div(s32, s32);
void Menu_EraseRect(s32, s32, s32, s32);
u32 Script_ParseStringRef(u32);
u32 Text_LookupString(u32);
void Menu_DrawText(s32, s32, u32);
void sub_081DFB18(u8 *p, s32 n)
{
    u8 tbl[4][2];
    s32 d, x, y;
    u32 text;
    memcpy(tbl, gUnk_0824FD3C, 8);
    d = Div(n, 6);
    x = (n - d * 6) * 2 + tbl[d][0];
    y = tbl[d][1];
    {
        u32 base = Script_ParseStringRef(gSelectionMenuParameters.pBlock);
        s32 off = n * 4;
        u8 *a = p + 0x770;
        text = Text_LookupString(base + *(u32 *)(a + off));
    }
    Menu_EraseRect(x, y, 2, 2);
    Menu_DrawText(x, y, text);
}
