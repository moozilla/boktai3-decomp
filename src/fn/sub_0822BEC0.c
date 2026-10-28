#include "global.h"
#include "runtime.h"
struct S {
    u8 p[8];
    u8 b;
};
u8 Save_WriteHeader(void);
u8 Save_ToggleSelectedSlot(void)
{
    struct S *s = gSessionState;
    s->b = 1 - s->b;
    return Save_WriteHeader();
}
