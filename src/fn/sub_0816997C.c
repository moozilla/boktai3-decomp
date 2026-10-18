#define SelectionPanel_RefreshPrimaryIfReady sub_0816997C

#include "global.h"
struct SelectionPanel {
    u8 pad[0xA5E];
    u8 state;
};
extern u32 gUnk_0300523C;
void Hud_DrawHealthMeter(struct SelectionPanel *, u32, u32, u32, u32);
void sub_08168EE8(struct SelectionPanel *);
void sub_0816997C(struct SelectionPanel *p)
{
    if (!(gUnk_0300523C & 1) && p->state <= 2) {
        Hud_DrawHealthMeter(p, 0, 4, 1, 1);
        sub_08168EE8(p);
    }
}
