#include "global.h"

void sub_0819AB64(u8 *, u32);

void sub_0819AB78(u8 *p, u32 v) {
    if (*(p + 0xE1) == 0xff) {
        sub_0819AB64(p, v);
    }
}
