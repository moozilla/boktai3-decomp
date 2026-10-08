#include "global.h"
extern u8 *gUnk_02000114;
void sub_08214514(u8 *);
void sub_0821FE6C(u8 *);
void sub_0821D6D0(u8 *);
u32 sub_0805173C(u8 *p)
{
    sub_08214514(p + 0x18);
    sub_0821FE6C(p + 0x60);
    sub_0821D6D0(p + 0xB4);
}
