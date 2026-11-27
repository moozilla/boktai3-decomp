#include "global.h"
#include "link/wireless/wireless_controller.h"
void Memory_Clear(void *, int);
#include "lib/rfu/rfu_compat.h"
#include "engine/ram.h"
int WirelessPacketQueue_Pop(void *, void *);
int WirelessController_PollInputExchange(struct Engine *p)
{
    u8 *changed;
    int result;
    void *key = &p->exchangeKey;
    void *message = p->packetQueue;
    if (WirelessPacketQueue_Pop(message, key)) {
        u16 *src;
        u16 *diff;
        u16 *history;
        int i;
        changed = &p->inputUpdated;
        src = p->exchangeInput;
        diff = p->inputChanges;
        history = p->inputHistory;
        i = 4;
        do {
            *history = *src;
            *diff ^= *src;
            src++;
            diff++;
            history++;
            i--;
        } while (i >= 0);
        result = 0;
    } else {
        result = -2;
        changed = &p->inputUpdated;
    }
    {
        *changed = 1;
    }
    return result;
}
