#include "global.h"
extern u32 gUnk_030053F4;
void sub_0822B358(s32);
void sub_08224F70(s32);
static inline void andm(u32 *a, u32 m) { *a &= m; }
void sub_081630BC(void)
{
    andm(&gUnk_030053F4, ~0x400);
    sub_0822B358(2);
    sub_08224F70(0);
}
