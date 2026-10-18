#include "global.h"
struct P {
    u8 pad[0x24];
    u32 w24;
};
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void Hud_SetMode(u32);
void Video_SetRenderCallbacks(u32, void *, void *, void *);
void Script_Call(u32, u32);
void sub_08217F4C(void);
void sub_08214B44(void);
void sub_08219624(void);
void WirelessTradeMenu_UpdateExit(u8 *p)
{
    u8 *f = p + 0x29;
    if (*f != 0) {
        u32 z = 0;
        *f = z;
        ScreenTransition_SendCommand(3, 3, 0, 0, z, 0xFFFF, z);
    }
    if (*(u32 *)(p + 0x24) == 10) {
        Hud_SetMode(7);
        Video_SetRenderCallbacks(0, sub_08217F4C, sub_08214B44, sub_08219624);
        if (*(u32 *)(p + 0xd4) != 0)
            Script_Call(*(u32 *)(p + 0xd4), 0);
    }
    *(u32 *)(p + 0x24) += 1;
}
