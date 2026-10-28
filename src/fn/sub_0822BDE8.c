#include "global.h"
struct S {
    u8 p[8];
    u8 b;
};
extern struct S *gSessionState;
u8 Save_ReadMainState(u32);
u8 Save_ReadContinuation(u32);
u32 Save_LoadAlternateSlot(void)
{
    u32 b = 1 - gSessionState->b;
    if (Save_ReadMainState(b)) {
        if (Save_ReadContinuation(b)) {
            gSessionState->b = b;
            return 1;
        }
    }
    return 0;
}
