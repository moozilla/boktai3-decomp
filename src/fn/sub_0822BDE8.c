#include "global.h"
#include "runtime.h"
struct S {
    u8 p[8];
    u8 b;
};
u8 Save_ReadMainState(u32);
u8 Save_ReadContinuation(u32);
u32 Save_LoadAlternateSlot(void)
{
    u32 b = 1 - ((struct S *)gSessionState)->b;
    if (Save_ReadMainState(b)) {
        if (Save_ReadContinuation(b)) {
            ((struct S *)gSessionState)->b = b;
            return 1;
        }
    }
    return 0;
}
