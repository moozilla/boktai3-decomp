#include "wireless_controller.h"
#include "rfu_compat.h"
#include "ram.h"
#include "rfu_manager.h"
void Wireless_UpdateUniAcknowledgedSlots(WirelessController *);
void Wireless_ReceiveParentUniPackets(WirelessController *);
void Wireless_ProcessChildUniExchange(WirelessController *);
void Wireless_RemoveConnectedSlots(WirelessController *, u8);
void WirelessController_ResetSession(WirelessController *);
int sub_082238F0(int);
void Wireless_OnParentUniSessionEvent(u8 command)
{
    int key = command;
    int saved = key;
    WirelessController *p = gWirelessController;
    if (!p) {
        goto done;
    }
    if (key == 0x41) {
        goto done;
    }
    if (key > 0x41) {
        goto high;
    }
    if (key == 0x30) {
        goto disconnected;
    }
    if (key > 0x30) {
        goto lowMore;
    }
    goto done;
lowMore:
    if (key == 0x32) {
        goto removed;
    }

    switch (key) {
        case 0x32 ... 0x7FFFFFFF:
            break;
        default:
            goto added;
    }

    if (key == 0x33) {
        goto disconnected;
    }
    if (key == 0x40) {
        goto lost;
    }
    goto done;
high:
    if (key == 0x50) {
        goto receive;
    }

    if (key > 0x50) {
        goto last;
    }
    if (key == 0x43) {
        goto done;
    }
    switch (key) {
        case 0x43 ... 0x7FFFFFFF:
            break;
        default:
            goto reset;
    }

    if (key == 0x44) {
        goto stop;
    }
    goto done;
last:
    if (key == 0xF1) {
        goto error;
    }

    if (key > 0xF1) {
        goto lastMore;
    }
    if (command == 0xF0) {
        goto error;
    }
    goto done;
lastMore:
    if (saved == 0xF2) {
        goto error;
    }

    if (saved == 0xFF) {
        goto error;
    }
    goto done;
receive:
    Wireless_UpdateUniAcknowledgedSlots(p);
    Wireless_ReceiveParentUniPackets(p);
    gWirelessSessionState = 0;
    goto done;
added: {
    u8 *slots = &p->selectedSlots;
    int incoming = (u8)gRfuLinkManager.event.words.code;
    *slots |= (u8)gRfuLinkManager.event.words.code;
}
    goto done;
removed: {
    u8 *slots = &p->selectedSlots;
    int incoming = (u8)gRfuLinkManager.event.words.code;
    *slots &= ~incoming;
}
    goto done;
disconnected: {
    p->selectedSlots &= ~gRfuLinkManager.event.bytes.bits;
    p->retransmitDone = 1;
    Wireless_RemoveConnectedSlots(p, gRfuLinkManager.event.bytes.bits);
    if (p->roster.count != 1) {
        goto done;
    }
    sub_082238F0(1);
    WirelessController_SetOperation(p, WIRELESS_OP_HANDLE_SEND_ERROR);
}
    goto done;
lost: {
    Wireless_RemoveConnectedSlots(p, (u8)gRfuLinkManager.event.words.code);
    if (p->roster.count != 1) {
        goto done;
    }
    sub_082238F0(1);
    WirelessController_SetOperation(p, WIRELESS_OP_HANDLE_SEND_ERROR);
}
    goto done;
reset:
    WirelessController_ResetSession(p);
    goto done;
stop:
    WirelessController_SetOperation(p, WIRELESS_OP_REQUEST_ADAPTER_STOP);
    goto done;
error:
    sub_082238F0(1);
    WirelessController_SetOperation(p, WIRELESS_OP_RESET_AFTER_ERROR);
    gWirelessSessionState = 0;
done:;
}
