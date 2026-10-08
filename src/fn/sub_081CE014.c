#include "global.h"
void sub_0802DB1C(void);
void sub_081CDE14(u8 *, u8 *);
void sub_081CDEF8(u8 *, u8 *);
s32 sub_081CE014(u8 *a, u8 *b)
{
    sub_0802DB1C();
    if (b[1] == 0x1d || b[1] == 9) {
        sub_081CDE14(a, b);
        sub_081CDEF8(a, b);
    }
    return 0;
}
