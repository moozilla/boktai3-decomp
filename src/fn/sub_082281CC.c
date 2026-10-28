#include "global.h"
#include "runtime.h"
struct G {
    u8 a[0x10];
    u32 w10;
};
u32 SaveData_IsHeaderValid(void)
{
    if (((struct G *)gSessionState)->w10 == 0x369F)
        return 1;
    return 0;
}
