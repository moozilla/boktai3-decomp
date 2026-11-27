#include "global.h"

u32 WirelessController_ConsumePeerRefreshRequest(u8 *p)
{
    u32 r;
    if (p[27] == 0) {
        r = 0;
    } else {
        p[27] = 0;
        r = 1;
    }
    return r;
}
