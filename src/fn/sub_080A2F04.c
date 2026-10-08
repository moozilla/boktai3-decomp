#include "global.h"
void sub_080A3C00(u8 *);
void sub_080A3E28(u8 *);
void sub_080A3EBC(u8 *);
void sub_08077C48(u8 *);
u32 sub_080A2F04(u8 *p)
{
    sub_080A3C00(p);
    sub_080A3E28(p);
    sub_080A3EBC(p);
    sub_08077C48(p);
    return 1;
}
