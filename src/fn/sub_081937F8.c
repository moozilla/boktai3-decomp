#include "global.h"

void sub_0821D6D0(u8 *);

void sub_081937F8(u8 *p) {
    if (*(p + 0x71BD)) {
        *(p + 0x71BD) = 0;
        sub_0821D6D0(p + 0x7100);
        sub_0821D6D0(p + 0x7110);
    }
}
