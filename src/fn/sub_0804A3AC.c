#include "global.h"
extern u8 *gUnk_02000488;
void sub_0804A3AC(void) {
    u8 *p = gUnk_02000488;
    if (p) p[0x473] = 1;
}
