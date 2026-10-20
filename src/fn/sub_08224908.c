#include "global.h"
#define gRfuSlotStatusNI gUnk_03006A70
#define gRfuLinkStatus   gUnk_03006A80
#include "rfu_compat.h"
struct Manager {
    u8 pad0[0x20];
    const u16 *allowed;
};
extern struct Manager gUnk_03005390;
#define RfuManager_FindAllowedParents sub_08224908

u8 RfuManager_FindAllowedParents(void)
{
    u8 result = 0;
    u8 i = result;
    while (i < gRfuLinkStatus->findParentCount) {
        const u16 *ids = gUnk_03005390.allowed;
        while (*ids != 0xffff) {
            if (gRfuLinkStatus->partner[i].serialNo == *ids)
                result |= 1 << i;
            ids++;
        }
        i++;
    }
    return result;
}
