#include "global.h"
extern u16 *sub_08163F70(u32);
void sub_08164738(void) {
    u16 *p = sub_08163F70(0);
    *p++ = 0xD040;
    *p++ = 0xD041;
    *p = 0xD042;
}
