#include "global.h"
extern u8 *gUnk_030053F8;
static inline void set_flag(u32 value, u8 *ptr)
{
    ptr[9] = value;
}
void sub_08227F3C(void)
{
    u32 value = 1;
    set_flag(value, gUnk_030053F8);
}
