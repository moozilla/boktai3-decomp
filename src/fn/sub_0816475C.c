#include "global.h"
extern u16 *sub_08163F70(u32);
void sub_0816475C(void) {
    u16 *p = sub_08163F70(0);
    *p++ = 0xD050;
    *p++ = 0xD051;
    *p = 0xD052;
}
