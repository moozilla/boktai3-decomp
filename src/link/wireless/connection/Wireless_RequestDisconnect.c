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
int WirelessController_ConsumeAdvanceRequest(struct Engine *);
void WirelessController_ResetSession(struct Engine *);
void Wireless_SetStatus(int);
int sub_082238F0(int);
int Wireless_RequestDisconnect(struct Engine *p)
{
    if (p->operationEntered) {
        p->operationEntered = 0;
        sub_082238F0(p->disconnectMode);
    }
    return 0;
}