#include "global.h"
struct P {
    u8 pad[0x19];
    u8 b19;
    u8 pad1[2];
    u32 w1c;
    u8 pad2[0xc];
    u32 w2c;
};
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void Sound_PlaySong(u32);
void Wireless_RequestResetConnection(void);
void Video_SetRenderCallbacks(u32, void *, void *, void *);
void Script_Call(u32, u32);
void sub_08217F4C(void);
void sub_08214B44(void);
void sub_08219624(void);
void WirelessStatusMenu_UpdateExitState(struct P *p)
{
    if (p->b19 != 0) {
        u32 z = 0;
        p->b19 = z;
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xFFFF, z);
        Sound_PlaySong(0xde);
        Wireless_RequestResetConnection();
    }
    if (p->w1c == 0x14) {
        Video_SetRenderCallbacks(0, sub_08217F4C, sub_08214B44, sub_08219624);
        if (p->w2c != 0)
            Script_Call(p->w2c, 0);
    }
    p->w1c++;
}
