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
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void LinkConnectionMenu_OnExitState(struct MessagePanel *);
void MessagePanel_BeginDismiss(struct MessagePanel *p)
{
    if (p->entering == 1) {
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xFFFF, 0);
        p->timer = 0;
        p->entering = 0;
    }
    if (p->timer == 60) {
        void (*next)(struct MessagePanel *) = LinkConnectionMenu_OnExitState;
        u32 mode = 4;
        p->state = next;
        p->entering = 1;
        p->mode = mode;
    } else
        p->timer++;
}
