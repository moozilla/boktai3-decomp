#include "global.h"
#include "link/wireless/wireless_controller.h"
#include "lib/rfu/rfu_compat.h"

int RfuManager_StartConnect(int, int, int, const u8 *);
extern const u8 gUnk_0824DE60[];

int Wireless_StartChildSearch(WirelessController *p)
{
    u8 status;
    if (p->operationEntered) {
        int i;
        p->operationEntered = 0;
        for (i = 0; i <= 3; i++)
            rfu_setRecvBuffer(32, i, p->exchangeWork, 4);
    }
    status = RfuManager_StartConnect(1, 0, 600, gUnk_0824DE60);
    if (status) {
        WirelessController_SetOperation(p, 27);
        return -1;
    }
    WirelessController_SetOperation(p, 7);
}
