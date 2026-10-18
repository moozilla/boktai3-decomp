#define MessagePanel_BeginDismiss sub_081098B0
#define MessagePanel_DelayDismiss sub_0810990C
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
void sub_081098B0(struct MessagePanel *);
void sub_0810990C(struct MessagePanel *p)
{
    if (p->entering == 1) {
        p->timer = 0;
        p->entering = 0;
    }
    if (p->timer == 120) {
        void (*next)(struct MessagePanel *) = sub_081098B0;
        u32 mode = 3;
        p->state = next;
        p->entering = 1;
        p->mode = mode;
    } else {
        p->timer++;
    }
}
