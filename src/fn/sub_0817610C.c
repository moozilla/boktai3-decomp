#define SelectionPanel_WaitThenSleep sub_0817610C
#define MenuPanel_SetCallback Hud_SetInteractionCallback
#define Power_EnterSleep sub_08227D70
// Provisional panel family; field meanings follow static consumers.
#include "global.h"
struct SelectionPanel {
    u8 unk0[0xA4E]; u16 timer;
    u8 unkA50[0x48BC - 0xA50]; u8 countdown;
    u8 unk48BD[0xB]; u32 window;
};
extern u32 gUnk_03005408;
void sub_08175854(u32);
void TextWindow_Clear(u32);
void sub_081660D0(void *, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_08176020(void);
void sub_08227D70(void);
void sub_0817610C(struct SelectionPanel *p)
{
    gUnk_03005408 = 1;
    if (++p->timer > 119) {
        sub_08175854(0x11);
        TextWindow_Clear(p->window);
        sub_081660D0(p, 1, -1);
        Hud_SetInteractionCallback(p, sub_08176020, 0);
        p->countdown = 30;
        sub_08227D70();
    }
}
