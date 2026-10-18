#include "global.h"
struct S {
    u8 p[8];
    u8 b;
};
extern struct S *gUnk_030053F8;
u8 Save_ReadMainState(u32);
u8 Save_ReadContinuation(u32);
u32 Save_LoadSelectedSlot(void)
{
    u32 b = gUnk_030053F8->b;
    if (Save_ReadMainState(b)) {
        if (Save_ReadContinuation(b))
            return 1;
    }
    return 0;
}
