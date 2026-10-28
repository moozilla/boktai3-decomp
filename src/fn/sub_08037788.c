#include "global.h"
struct Row {
    u8 pad0[0x58];
    u32 flags;
    u8 pad5c[4];
};
struct PeerMenu {
    u8 pad0[0x19];
    u8 state, entered, pad1b, previous, pad1d[3];
    u32 timer;
    u8 pad24[0x264];
    struct Row rows[5];
    u8 pad468[0x64];
    u8 name[24];
    u8 pad4e4[8];
    int window;
    u8 pad4f0[0x104];
    u8 draw[8];
};
#include "rfu_compat.h"
#include "input.h"
int Wireless_GetConnectedSlotMask(void);
int Wireless_GetPeerHandshakeData(void);
void TextWindow_Refresh(int, int);
void Memory_Clear(void *, int);
void WirelessText_DecodeString(const void *, void *, int);
void Anim_DrawDecimalDigits(void *, void *, int, int, int, int, int, int, int);
void TextWindow_SetString(int, int, void *);
void Sound_PlaySong(int);
void Wireless_RequestAdvanceConnection(void);
void Wireless_RequestDisconnectPeer(void);
void WirelessPeerMenu_UpdatePeerConfirmation(struct PeerMenu *p)
{
    u8 alias;
    u8 connected = Wireless_GetConnectedSlotMask();
    struct Row *r;
    int i;
    if (p->entered) {
        p->entered = 0;
        TextWindow_Refresh(p->window, 7);
        p->previous = 255;
    }
    if (connected != p->previous) {
        r = p->rows;
        if (connected && (connected == gRfuLinkStatus->connSlotFlag)) {
            if (Wireless_GetPeerHandshakeData()) {
                u8 *name;
                void *draw;
                int bit;
                i = 0;
                alias = gRfuLinkStatus->connSlotFlag;
                {
                    int tested = alias & 1;
                    bit = tested;
                }
                name = p->name;
                draw = ((u8 *)p) + 0x2d8;
                if (!bit) {
                    struct RfuLinkStatus **links = &gRfuLinkStatus;
                    /* do-while(0) only keeps agbcc's block layout */
                    do {
                        i++;
                        if (i > 3) {
                            break;
                        }
                    } while (
                        !(((*((struct RfuLinkStatus *volatile *)links))->connSlotFlag >> i) & 1));
                }
                r = p->rows;
                Memory_Clear(name, 24);
                WirelessText_DecodeString(gRfuLinkStatus->partner[i].gname, name, 11);
                {
                    u8 *packet = gRfuLinkStatus->partner[i].gname;
                    int value = packet[11] | (packet[12] << 8);
                    Anim_DrawDecimalDigits(draw, p->draw, value, 5, 0, 136, 70, 8, 0);
                }
                {
                    u32 mask = ~1;
                    struct Row *cursor = r;
                    i = 4;
                    /* do-while(0) only keeps agbcc's block layout */
                    do {
                        cursor->flags &= mask;
                        cursor++;
                    } while ((--i) >= 0);
                }
                TextWindow_SetString(p->window, 2, ((u8 *)r) + 0x244);
                Sound_PlaySong(495);
            }
            TextWindow_Refresh(p->window, 8);
        } else {
            TextWindow_Refresh(p->window, 7);
            {
                u32 mask = 1;
                struct Row *cursor = r;
                i = 4;
                /* do-while(0) only keeps agbcc's block layout */
                do {
                    cursor->flags |= mask;
                    cursor++;
                } while ((--i) >= 0);
            }
        }
        p->previous = connected;
    }
    if ((gPadState[1] & 1) && connected) {
        Wireless_RequestAdvanceConnection();
    }
    if (!r) {
    }
    if (gPadState[1] & 2) {
        if (connected) {
            Wireless_RequestDisconnectPeer();
        } else {
            Wireless_RequestAdvanceConnection();
            p->state = 7;
            p->entered = 1;
            p->timer = connected;
        }
    }
    p->timer++;
}
