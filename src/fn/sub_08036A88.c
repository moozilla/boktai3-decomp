#include "global.h"
#include "ram.h"

struct S {
    u8 f[0x134];
    u8 a;
};
u8 *Wireless_GetPeerHandshakeData(void)
{
    struct S *p = gWirelessController;
    if (!p)
        return 0;
    return &p->a;
}
