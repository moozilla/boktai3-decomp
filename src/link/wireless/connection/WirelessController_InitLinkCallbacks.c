#include "global.h"
#include "link/wireless/wireless_controller.h"
void Memory_Clear(void *, int);
#include "lib/rfu/rfu_compat.h"
#include "engine/ram.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->operationStage = zero;
    p->operation = op;
    p->operationFrame = zero;
    p->operationEntered = 1;
}
void WirelessController_OnLinkEvent(u8);
void WirelessController_OnReceiveRequest(void);
int RfuManager_Init(void *, void *);
int WirelessController_InitLinkCallbacks(struct Engine *p)
{
    if (p->operationEntered)
        p->operationEntered = 0;
    {
        int status = (u8)RfuManager_Init(WirelessController_OnLinkEvent,
                                         WirelessController_OnReceiveRequest);
        if (status)
            setOperation(p, 27, 0);
        else {
            setOperation(p, 2, status);
            p->managerReady = 1;
        }
    }
    return 0;
}
