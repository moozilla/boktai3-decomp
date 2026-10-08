#include "global.h"

void sub_08226178(void) {
    u8 *p = *(u8 **)0x03005420;
    if (p != 0) {
        *(u16 *)(p + 0x74) = 0;
        *(u16 *)(p + 0x76) = 0;
        *(u16 *)(p + 0x78) = 0;
    }
}
