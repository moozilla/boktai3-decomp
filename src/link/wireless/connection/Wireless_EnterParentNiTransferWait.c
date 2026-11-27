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
int Wireless_EnterParentNiTransferWait(struct Engine *p)
{
    u8 *pending = &p->operationEntered;
    if (*pending)
        *pending = 0;
    setOperation(p, 23, 0);
    *pending = 1;
    return 0;
}
