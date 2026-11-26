#include "engine/shared/wireless_controller.h"
#include "lib/rfu/rfu_compat.h"
#include "engine/shared/ram.h"
int WirelessController_ConsumeAdvanceRequest(WirelessController *);
void WirelessController_ResetSession(WirelessController *);
void Wireless_SetStatus(int);
int sub_082238F0(int);
int Wireless_HandleSendError(WirelessController *p)
{
    if (p->operationEntered) {
        p->operationEntered = 0;
        Wireless_SetStatus(2);
        sub_082238F0(1);
        p->refreshRequested = 0;
        p->operationStage = 0;
    }
    if (WirelessController_ConsumeAdvanceRequest(p))
        WirelessController_SetOperation(p, WIRELESS_OP_REQUEST_ADAPTER_STOP);
    return 0;
}