#include "engine/engine.h"

#include "engine/ram.h"
#include "link/wireless/wireless_controller.h"

void WirelessController_ResetSession(WirelessController *p)
{
    int i;
    p->operationStage = 0;
    p->connectedSlots = 0;
    p->previousConnectedSlots = 0;
    p->unknown1E = 0;
    p->localPeerIndex = 0;
    p->sendSlot = 0;
    p->parentSlot = 0;
    p->niFlags = 0;
    p->peerScroll = 0;
    p->selectedSlots = 0;
    p->availableSlots = 0;
    p->availablePeerCount = 0;
    for (i = 0; i < 8; i++)
        p->availablePeerIndices[i] = 0;
    p->isParent = 1;
    p->inputReady = 0;
    p->sessionMode = 0;
    p->lostSlots = 0;
    p->sendReady = 0;
    p->operationStage = 0;
    p->operation = 0;
    p->operationFrame = 0;
    p->operationEntered = 1;
    p->unknown3A = 0;
    p->adapterDelay = 0;
    p->resetRequested = 0;
    p->receivedInput = 0;
    p->disconnectMode = 0;
    p->timer = 0;
    p->unknown42 = 0;
    p->discoveredPeerCount = 0;
    p->frameCounter = 0;
    gWirelessSessionState = 0;
    p->unknownFB = 0;
    p->sessionMode = 0;
    p->onSend = 0;
    p->onReceive = 0;
    Memory_Clear(&p->roster, sizeof(p->roster));
    Memory_Clear(&p->exchangeValue, sizeof(p->exchangeValue));
    Memory_Clear(p->exchangeWork, sizeof(p->exchangeWork));
}
