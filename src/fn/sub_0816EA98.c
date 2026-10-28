#define MenuTransition_SetCallback Hud_SetInteractionCallback
#include "global.h"
#include "input.h"
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MessageWindow_Clear(void);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_ShowMessage(s32);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
struct MenuMessageView {
    u8 pad[0x3c];
    void *resource;
    u8 unknown40[0xa4e - 0x40];
    u16 timer;
};
void MenuTransition_UpdateSelectionFade(void);
void MenuTransition_UpdateSelectionMessage(u8 *p)
{
    u16 *timer = &((struct MenuMessageView *)p)->timer;
    if (*timer == 0) {
        MessageWindow_SetMessageTable(((struct MenuMessageView *)p)->resource);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetBounds(0, 16, 30, 4);
        MessageWindow_ShowMessage(p[0x4861] + 0x6e);
    }
    ++*timer;
    if (*timer > 89 || (*timer > 15 && (gPadState[1] & 1))) {
        u32 index = p[0x438a] + 10;
        u32 off = index * 0x60;
        u8 *base = p + 0x1428;
        u32 *flags = (u32 *)(base + off);
        *flags |= 1;
        MessageWindow_Clear();
        Hud_SetInteractionCallback(p, MenuTransition_UpdateSelectionFade, 1);
    }
}
