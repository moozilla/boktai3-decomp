#include "global.h"
#define gRfuSlotStatusNI gUnk_03006A70
#define gRfuLinkStatus   gUnk_03006A80
#include "rfu_compat.h"
#include "rfu_manager.h"

u8 RfuManager_FindAllowedParents(void)
{
    u8 result = 0;
    u8 i = result;
    while (i < gRfuLinkStatus->findParentCount) {
        const u16 *ids = gRfuLinkManager.acceptable_serialNo_list;
        while (*ids != 0xffff) {
            if (gRfuLinkStatus->partner[i].serialNo == *ids)
                result |= 1 << i;
            ids++;
        }
        i++;
    }
    return result;
}
