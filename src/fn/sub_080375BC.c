#include "global.h"
struct PeerPanel {
    u8 active, pad1[0x22f];
    u8 name[16];
};
struct PeerMenu {
    u8 pad0[0x4e4];
    int window;
    u8 pad4e8[12];
    u32 clear;
    u8 pad4f8[0xfc];
    u8 draw[8];
};
int Wireless_GetPeerHandshakeData(void);
int Wireless_GetLocalSessionId(void);
int Wireless_GetConnectedSlotMask(void);
void Anim_DrawDecimalDigits(void *, void *, int, int, int, int, int, int, int);
void WirelessPeerPanel_SetAvailable(void *, void *);
void WirelessPeerPanel_SetUnavailable(void *, void *);
void *WirelessPeerMenu_GetFirstConnectedPlayer(int);
void *WirelessExchange_GetPeerEntry(int);
void Memory_Copy(void *, const void *, int);
void TextWindow_Refresh(int, int);
void WirelessPeerPanel_SetNameFromPacket(void *, void *, int, void *, int);
void WirelessPeerPanel_ClearName(void *, void *, int);
extern u8 *gGameState;
void WirelessPeerPanel_Update(struct PeerMenu *p, struct PeerPanel *panel, int index)
{
    int status = Wireless_GetPeerHandshakeData();
    void *peer;
    if (!index) {
        int value = Wireless_GetLocalSessionId();
        Anim_DrawDecimalDigits((u8 *)panel + 0x50, p->draw, value, 5, 0, 24, 70, 8, 0);
        goto available;
    }
    if (Wireless_GetConnectedSlotMask()) {
    available:
        WirelessPeerPanel_SetAvailable(p, panel);
    } else
        WirelessPeerPanel_SetUnavailable(p, panel);
    if (status) {
        peer = WirelessPeerMenu_GetFirstConnectedPlayer(status);
        if (!index) {
            if (!panel->active) {
                void *name = WirelessExchange_GetPeerEntry(0);
                u32 zero = 0;
                CpuSet(&zero, &p->clear, 0x05000001);
                Memory_Copy(name, gGameState + 0x780, 16);
                Memory_Copy(panel->name, gGameState + 0x780, 16);
                TextWindow_Refresh(p->window, 3);
                panel->active = 1;
            }
        } else if (peer) {
            if (!panel->active) {
                WirelessPeerPanel_SetNameFromPacket(p, panel, index, peer, status);
                panel->active = 1;
            }
        } else if (panel->active) {
            WirelessPeerPanel_ClearName(p, panel, index);
            panel->active = 0;
        }
    }
}
