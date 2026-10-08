#include "global.h"

void sub_081A3828(u8 *);

void sub_081A3D90(u32 a, u32 b, u8 *p) {
    if (*(u16 *)(p + 0x15E)) {
        sub_081A3828(p);
    }
}
