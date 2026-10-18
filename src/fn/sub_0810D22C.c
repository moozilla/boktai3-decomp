#include "global.h"
struct PanelTransition {
    u8 unknown0[0x140C];
    u8 entered;
    u8 unknown140D;
    u16 frame;
};
void Task_Kill(void *);
void Registry1745_SetMenuRequest(u32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void PanelTransition_UpdateExit(struct PanelTransition *p)
{
    if (p->entered) {
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xFFFF, 0);
        p->frame = 0;
        p->entered = 0;
    }
    if (p->frame > 15) {
        Registry1745_SetMenuRequest(1);
        Task_Kill(p);
    } else {
        p->frame++;
    }
}
