#include "global.h"

struct S {
    u8 f[0x32];
    u8 a;
};
void RfuManager_RequestStopMode(void);
u32 Wireless_RequestAdapterStop(struct S *p)
{
    if (p->a) {
        p->a = 0;
        RfuManager_RequestStopMode();
    }
    return 0;
}
