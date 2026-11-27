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
void RfuManager_StartConfiguration(void *);
int WirelessController_StartConfiguredLink(struct Engine *p)
{
    if (p->operationEntered)
        p->operationEntered = 0;
    if (p->operationFrame > 59) {
        RfuManager_StartConfiguration((u8 *)p + 0x11C);
        gWirelessSessionState = 0;
        setOperation(p, 4, 0);
    }
    return 0;
}
