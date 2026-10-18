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
void MessagePanel_BeginDismiss(struct MessagePanel *);
void MessagePanel_WaitForDismiss(struct MessagePanel *p)
{
    if (p->entering) {
        TextWindow_SetScript(p->window, p->script);
        TextWindow_Refresh(p->window, 5);
        Sound_PlaySong(0x2AE);
        p->timer = 0;
        p->entering = 0;
    }
    if (p->timer == 120) {
        void (*next)(struct MessagePanel *) = MessagePanel_BeginDismiss;
        u32 mode = 3;
        p->state = next;
        p->entering = 1;
        p->mode = mode;
    } else {
        p->timer++;
    }
}
