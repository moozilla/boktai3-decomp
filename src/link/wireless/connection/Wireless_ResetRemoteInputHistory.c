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
void Wireless_ResetRemoteInputHistory(struct Engine *p)
{
    u16 *history = (u16 *)((u8 *)p + 0xEA);
    int bits;
    int i;
    {
        int initial = 65535;
        *history++ = initial;
        initial = -1;
        *history = initial;
        bits = initial;
    }
    history += 2;
    i = 4;
    do {
        int value = *history;
        value |= bits;
        *history++ = value;
    } while (--i >= 0);
}
