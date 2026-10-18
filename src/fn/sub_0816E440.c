#define MenuTransition_UpdateTimedMessage sub_0816E440
#define gMenuTransitionKeys               gUnk_03005260
#define MenuTransition_SetCallback Hud_SetInteractionCallback
#include "global.h"
void sub_0816D98C(void *);
void sub_0816FF4C(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0816CDB8(void *, int, int);
void sub_0816D920(void *, int);
void sub_0816D868(void *);
void Hud_SaveGridSelection(void *);
void sub_081672FC(void *);
void sub_08166B7C(void *, int);
void sub_0816701C(void *, int, int, int, int, int);
void sub_081663A0(void *, int, int, int, int);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_ShowMessage(s32);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
int MessageWindow_SetNumberArgument(int, int);
extern u16 gUnk_03005260[];
struct MenuMessageView {
    u8 pad[0x3c];
    void *resource;
    u8 unknown40[0xa4e - 0x40];
    u16 timer;
};
void sub_0816E440(u8 *p)
{
    u16 *timer = &((struct MenuMessageView *)p)->timer;
    u8 *q, *a;
    u32 r;
    if (*timer == 0) {
        MessageWindow_SetMessageTable(((struct MenuMessageView *)p)->resource);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetBounds(0, 16, 30, 4);
        MessageWindow_ShowMessage(p[0x4862] + 0x74);
        MessageWindow_SetNumberArgument(0, p[0x4863]);
    }
    {
        u32 next = *timer + 1;
        r = 0;
        *timer = next;
    }
    if (*timer > 89 || (*timer > 15 && (gUnk_03005260[1] & 1))) {
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetBounds(0, 16, 30, 2);
        sub_0816D98C(p);
        Hud_SetInteractionCallback(p, sub_0816FF4C, 1);
        q = p + 0x4384;
        sub_0816CDB8(q, 1, 0);
        a = p + 0x1620;
        sub_0816D920(a, p[0x4387]);
        sub_0816D868(p);
        Hud_SaveGridSelection(q);
        sub_081672FC(p);
        sub_08166B7C(p, 4);
        sub_0816701C(p, 0, 0, 0, 1, r);
        sub_081663A0(p, 1, 0, 0, r);
    }
}
