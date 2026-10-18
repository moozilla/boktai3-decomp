#define SelectionPanel_ProcessChoice sub_081760A4
#define SelectionPanel_WaitThenSleep sub_0817610C
#define MenuPanel_SetCallback Hud_SetInteractionCallback
// Provisional panel family; field meanings follow static consumers.
#include "global.h"
struct SelectionPanel { u8 unk0[0x48C8]; u32 window; };
int sub_08165A30(void *);
void Sound_PlaySong(u32);
void sub_08175854(u32);
void TextWindow_Clear(u32);
void sub_081660D0(void *, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_08176020(void);
void sub_0817610C(void);
void sub_081760A4(struct SelectionPanel *p)
{
    int choice = sub_08165A30(p);
    if (choice == 0) {
        Sound_PlaySong(0xDE);
        sub_08175854(0x11);
        TextWindow_Clear(p->window);
        sub_081660D0(p, 1, -1);
        Hud_SetInteractionCallback(p, sub_08176020, 0);
    } else if (choice == 1) {
        Sound_PlaySong(0xDD);
        sub_08175854(0x13);
        Hud_SetInteractionCallback(p, sub_0817610C, 1);
    }
}
