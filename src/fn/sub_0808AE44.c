#include "global.h"

u32 sub_08074600(u32, u32);

void sub_0808AE44(u8 *p) {
    *(u32 *)(p + 0x308) = sub_08074600(4, 0);
}
