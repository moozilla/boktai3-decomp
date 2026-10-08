#include "global.h"

void sub_081A3380(u8 *);

void sub_081A349C(u32 a, u32 b, u8 *p) {
    if (*(u16 *)(p + 0x8A)) {
        sub_081A3380(p);
    }
}
