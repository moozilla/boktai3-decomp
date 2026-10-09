#include "global.h"
extern u16 *sub_08163F70(u32);
void sub_08164780(void) {
    u16 *p = sub_08163F70(0);
    *p++ = 0xD08D;
    *p++ = 0xD08E;
    *p = 0xD08F;
}
