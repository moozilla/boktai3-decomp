#include "engine.h"
#include "runtime.h"
#include "wireless_controller.h"

int Wireless_PublishLocalInput(WirelessController *p)
{
    u32 input;

    if (p->operationEntered) {
        p->operationEntered = 0;
        Bg_ClearTilemap(0);
    }
    input = gHardwareKeys;
    input |= WIRELESS_INPUT_VALID;
    WirelessController_SetLocalInput(p, input);
    p->inputReady = 1;
}
