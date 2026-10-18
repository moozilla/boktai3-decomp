#define gMessagePanelInput gUnk_03005260
#include "global.h"
#include "engine.h"
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
extern u16 gUnk_03005260[];
void sub_08224FA4(void);
void MessagePanel_WaitForInput(struct MessagePanel *p)
{
    if (p->entering) {
        TextWindow_SetScript(p->window, p->script);
        TextWindow_Refresh(p->window, 3);
        Sound_PlaySong(0x192);
        p->timer = 0;
        p->entering = 0;
    }
    if (p->timer < 60) p->timer++;
    else if (gUnk_03005260[1] & 0x30F) sub_08224FA4();
}
