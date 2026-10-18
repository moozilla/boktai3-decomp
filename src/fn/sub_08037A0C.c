#include "global.h"
struct P {
    u8 pad[0x1a];
    u8 b1a;
    u8 pad1[5];
    u32 w20;
    u8 pad2[0x4cc];
    u32 w4f0;
};
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void Sound_PlaySong(u32);
void Wireless_RequestResetConnection(void);
void Script_Call(u32, u32);
void WirelessPeerMenu_UpdateExit(struct P *p)
{
    if (p->b1a != 0) {
        p->b1a = 0;
        Sound_PlaySong(0xde);
        Wireless_RequestResetConnection();
    }
    if (p->w20 == 4) {
        ScreenTransition_SendCommand(3, 4, 4, 4, p->w20, 0xFFFF, 0);
    }
    if (p->w20 == 0x18) {
        if (p->w4f0 != 0)
            Script_Call(p->w4f0, 0);
    }
    p->w20++;
}
