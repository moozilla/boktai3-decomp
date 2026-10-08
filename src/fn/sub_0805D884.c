#include "global.h"
void sub_08214514(void *);
void sub_0821FE6C(void *);
void sub_0823233C(void *);
u32 sub_0805D884(u8 *p) {
    sub_08214514(p + 0x18);
    sub_0821FE6C(p + 0x68);
    sub_0823233C(p + 0x104);
    return 0;
}
