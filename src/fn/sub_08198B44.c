#include "global.h"

void sub_08013B74(u8 *);
void sub_0821FE6C(u8 *);
void sub_082195E0(u8 *);

void sub_08198B44(u8 *p) {
    sub_08013B74(p + 0x88);
    sub_0821FE6C(p + 0xF4);
    if (*(p + 0x2C)) {
        sub_082195E0(p + 0x28);
    }
}
