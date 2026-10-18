#define LinkMenu_UpdateExitDelay sub_08110378
#define LinkSession_RequestExit Registry1745_RequestHandshakeFinish
#include "global.h"
#include "engine.h"
struct LinkMenuExit {
    u8 unknown0[0xFF4];
    const u8 *script;
    u8 unknownFF8[0x10];
    s32 textWindow;
    u8 unknown100C[0x18];
    u8 entering;
    u8 unknown1025;
    u16 timer;
};
void Registry1745_RequestHandshakeFinish(void);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void sub_08110378(struct LinkMenuExit *menu)
{
    if (menu->entering) {
        TextWindow_SetScript(menu->textWindow, menu->script);
        TextWindow_Refresh(menu->textWindow, 5);
        Sound_PlaySong(0x2AE);
        menu->timer = 0;
        menu->entering = 0;
    }
    if (menu->timer == 120) {
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xFFFF, 0);
    } else if (menu->timer > 140) {
        Registry1745_RequestHandshakeFinish();
        return;
    }
    menu->timer++;
}
