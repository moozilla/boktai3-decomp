#include "link/wireless/wireless_controller.h"
#include "lib/rfu/rfu_compat.h"
#include "engine/ram.h"
#include "link/wireless/rfu_manager.h"
void Wireless_UpdateUniAcknowledgedSlots(WirelessController *);
void Wireless_ReceiveParentUniPackets(WirelessController *);
void Wireless_ProcessChildUniExchange(WirelessController *);
void Wireless_RemoveConnectedSlots(WirelessController *, u8);
void WirelessController_ResetSession(WirelessController *);
int sub_082238F0(int);
void Wireless_OnChildUniSessionEvent(u8 command)
{
    int key = command;
    int saved = key;
    WirelessController *p = gWirelessController;
    if (!p) {
        goto done;
    }
    if (key > 0x41) {
        goto high;
    }
    switch (key) {
        case 0x40 ... 0x7FFFFFFF:
            goto done;
        default:
            break;
    }

    if (key == 0x30) {
        goto disconnected;
    }
    if (key <= 0x30) {
        goto done;
    }
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
    goto done;
high:
    if (key == 0x50) {
        goto receive;
    }

    if (key > 0x50) {
        goto last;
    }
    if (key == 0x42) {
        goto reset;
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
        goto checkError;
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
    Wireless_ProcessChildUniExchange(p);
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
    sub_082238F0(1);
    WirelessController_SetOperation(p, WIRELESS_OP_HANDLE_SEND_ERROR);
}
    goto done;
reset:
    WirelessController_ResetSession(p);
    goto done;
checkError: {
    struct RfuLinkManager *event;
    struct RfuLinkManager *initial = &gRfuLinkManager;
    int code = initial->event.words.code;
    event = initial;
    if (code != 0x30) {
        goto valueCheck;
    }
    if (event->event.words.value != 3) {
        goto valueCheck;
    }
    event->event.words.value = 0;
valueCheck:
    if (!event->event.words.value) {
        goto done;
    }
}
error:
    sub_082238F0(1);
    WirelessController_SetOperation(p, WIRELESS_OP_RESET_AFTER_ERROR);
done:;
}
