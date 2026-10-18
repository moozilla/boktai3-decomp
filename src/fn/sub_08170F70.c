#include "global.h"
s32 Inventory_GetConsumableId(s32);
void sub_08170F54(void *);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void Anim_LoadKeyframe(void *, void *, u16, s32);
void MessageWindow_SetDisplayMode(s32);
void MessageWindow_SetMessageTable(u32);
void MessageWindow_SetBounds(s32, s32, s32, s32);
void MessageWindow_ShowMessage(s32);
void ScrollingDialogue_RequestShow(void);
u8 *Script_ParseStringRef(u32);
void *Text_LookupString(void *);
void ScrollingDialogue_SetScript(void *);
void PauseInventory_RefreshConsumableDescription(u8 *p)
{
    s32 r;
    if (p[0x4387] <= 15) {
        r = Inventory_GetConsumableId(p[0x4387]);
        if (r == 255) {
            r = -1;
            sub_08170F54(p);
        }
    } else {
        sub_08170F54(p);
        r = -1;
    }
    {
        u32 *q = (u32 *)(p + 0x2a48);
        u32 v = *q, m = 1;
        v |= m;
        *q = v;
        if (r < 0) {
            MessageWindow_Clear();
            ScrollingDialogue_ClearScript();
        } else {
            v &= ~1;
            *q = v;
            Anim_LoadKeyframe(p + 0x2a40, p + 0xa4, 0xcf, 1);
            MessageWindow_SetDisplayMode(1);
            MessageWindow_SetMessageTable(*(u32 *)(p + 0x44));
            MessageWindow_SetBounds(2, 16, 28, 2);
            MessageWindow_ShowMessage(r);
            ScrollingDialogue_RequestShow();
            {
                u8 *base = Script_ParseStringRef(*(u32 *)(p + 0x44)) + 0x30;
                u8 **base_pointer = &base;
                ScrollingDialogue_SetScript(Text_LookupString(*base_pointer + r));
            }
        }
    }
}
