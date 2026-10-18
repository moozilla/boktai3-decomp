#define SelectionPanel_ConfirmTransition sub_08176224
#define SelectionPanel_ProcessSecondChoice sub_08176308
#define MenuPanel_SetCallback Hud_SetInteractionCallback
// Provisional panel family; field meanings follow static consumers.
#include "global.h"
int sub_08165A30(void *);
void Sound_PlaySong(u32);
void sub_08175854(u32);
void sub_081660D0(void *, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_08176224(void);
void sub_0817635C(void);
void sub_08176308(void *p)
{
    int choice = sub_08165A30(p);
    if (choice == 0) {
        Sound_PlaySong(0xDE);
        sub_08175854(8);
        sub_081660D0(p, 1, -1);
        Hud_SetInteractionCallback(p, sub_08176224, 0);
    } else if (choice == 1) {
        Sound_PlaySong(0xDD);
        Hud_SetInteractionCallback(p, sub_0817635C, 1);
    }
}
