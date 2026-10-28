#include "global.h"
struct G {
    u8 a[0x10];
    u32 w10;
};
extern struct G *gSessionState;
u32 SaveData_IsHeaderValid(void)
{
    if (gSessionState->w10 == 0x369F)
        return 1;
    return 0;
}
