#include "global.h"

void sub_081F0734(u8 *);

void sub_081F1260(u8 *p) {
    *(u8 *)(p + 0xAA) = 0;
    sub_081F0734(p);
}
