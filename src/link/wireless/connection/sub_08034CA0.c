#include "global.h"
#include "link/wireless/wireless_controller.h"
#include "lib/rfu/rfu_compat.h"

int RfuManager_StartConnect(int, int, int, const u8 *);
extern const u8 gUnk_0824DE60[];

int Wireless_StartParentSearch(WirelessController *p)
{
    u8 status;
    if (p->operationEntered) {
        p->operationEntered = 0;
        p->availableSlots = 0;
    }
    status = RfuManager_StartConnect(0, 0, 600, gUnk_0824DE60);
    if (status) {
        WirelessController_SetOperation(p, 27);
        return -1;
    }
    WirelessController_SetOperation(p, 8);
}
