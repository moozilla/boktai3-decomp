#include "global.h"
extern u8 *gUnk_02000484;
void sub_08036AFC(void) {
    u8 *p = gUnk_02000484;
    if (p) p[0x1b] = 1;
}
