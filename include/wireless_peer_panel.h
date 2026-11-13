#ifndef GUARD_WIRELESS_PEER_PANEL_H
#define GUARD_WIRELESS_PEER_PANEL_H

#include "wireless_status_menu.h"
#include "wireless_text.h"
#include "contact_swarm_sprite.h"
#include "display_sequence.h"
#include "rfu_compat.h"

struct WirelessPeerStyle {
    s32 resource;
    u8 paletteBank, animation, priority, x, y;
    u8 unknown09[3];
};

struct WirelessPeerPanel {
    u8 active;
    u8 unknown01[3];
    u32 flags;
    struct ContactSwarmSprite sprite;
    u8 graphics[0x1C];
    struct SceneInputWidget digits[5];
    u8 name[16];
    u8 unknown240[4];
    u8 confirmationName[24];
};

struct WirelessPeerMenu {
    struct Task task;
    u8 operation, state, entered, unknown1B, previousSlots;
    u8 unknown1D[3];
    u32 timer;
    void *background, *script;
    struct WirelessPeerPanel panels[2];
    s32 windows[3];
    u32 completionProcedure;
    u8 scratch[0x100];
    struct SceneInputResource digitResource, iconResource;
    struct SceneInputWidget icon;
};

extern const struct WirelessPeerStyle gUnk_0824DE64[];
void Anim_RestartSequence(void *, void *, int, int, int);
void *WirelessExchange_GetPeerEntry(int);
int Wireless_GetConnectedSlotMask(void);
const u8 *Wireless_GetPeerHandshakeData(void);
int Wireless_GetLocalSessionId(void);
int Wireless_IsSessionActive(void);
void Wireless_RequestAdvanceConnection(void);
void Wireless_RequestDisconnectPeer(void);
int Wireless_GetOperation(void);
void Anim_DrawDecimalDigits(void *, void *, int, int, int, int, int, int, int);
void TextWindow_SetString(int, int, void *);
void WirelessPeerMenu_InitBackground(struct WirelessPeerMenu *p, int layer);
int WirelessPeerPanel_Init(struct WirelessPeerMenu *owner, struct WirelessPeerPanel *panel,
                           int kind);
void WirelessPeerPanel_Destroy(void *owner, struct WirelessPeerPanel *p);
u32 WirelessPeerPanel_SetAvailable(void *owner, struct WirelessPeerPanel *p);
u32 WirelessPeerPanel_SetUnavailable(void *owner, struct WirelessPeerPanel *p);
int WirelessPeerPanel_SetNameFromPacket(void *ownerArg, struct WirelessPeerPanel *panel,
                                        int peerArg, int playerArg, const u8 *packetArg);
int WirelessPeerPanel_ClearName(struct WirelessPeerMenu *owner, struct WirelessPeerPanel *p,
                                int peer);
int WirelessPeerMenu_GetFirstConnectedPlayer(const u8 *handshake);
void WirelessPeerPanel_Update(struct WirelessPeerMenu *p, struct WirelessPeerPanel *panel,
                              int index);
void WirelessPeerMenu_UpdateCancelInput(struct WirelessPeerMenu *p);
void WirelessPeerMenu_UpdateDismissibleNotice(struct WirelessPeerMenu *p);
void WirelessPeerMenu_UpdatePeerConfirmation(struct WirelessPeerMenu *p);
void WirelessPeerMenu_UpdateStatusNotice(struct WirelessPeerMenu *p);
void WirelessPeerMenu_UpdatePeerUnavailable(struct WirelessPeerMenu *p);
void WirelessPeerMenu_UpdateStatusElevenNotice(struct WirelessPeerMenu *p);
void WirelessPeerMenu_UpdateStatusTwentyEightNotice(struct WirelessPeerMenu *p);
void WirelessPeerMenu_UpdateExit(struct WirelessPeerMenu *p);
void WirelessPeerMenu_UpdateConnectionState(struct WirelessPeerMenu *p);

#endif
