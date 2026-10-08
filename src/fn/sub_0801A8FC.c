#include "global.h"
void sub_0822B2F8(u32);
void sub_08019F18(u8 *, void (*)(void));
void sub_0801AF54(void);
void sub_0801A8FC(u8 *p)
{
    if (p[0x1e] <= 1) {
        sub_0822B2F8(0xdd);
        sub_08019F18(p, sub_0801AF54);
    }
}
