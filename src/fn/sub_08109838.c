#include "global.h"
#include "engine.h"
#include "input.h"
struct MessagePanel {
    u8 unk0[0x20];
    void (*state)(struct MessagePanel *);
    const u8 *script;
    u8 unk28[8];
    s32 window;
    u8 mode;
    u8 entering;
    u16 timer;
};
void System_ShutdownAndSoftReset(void);
void MessagePanel_WaitForInput(struct MessagePanel *p)
{
    if (p->entering) {
        TextWindow_SetScript(p->window, p->script);
        TextWindow_Refresh(p->window, 3);
        Sound_PlaySong(0x192);
        p->timer = 0;
        p->entering = 0;
    }
    if (p->timer < 60)
        p->timer++;
    else if (gPadState[1] & 0x30F)
        System_ShutdownAndSoftReset();
}
