#include "global.h"
void sub_0822D080(u32, u32);
void sub_0822D0C8(u32, u32);
void sub_0816D7F8(u8 *s, u32 a, u32 b) {
    if (s[0x4860] == 0)
        sub_0822D080(a, b);
    else
        sub_0822D0C8(a, b);
}
