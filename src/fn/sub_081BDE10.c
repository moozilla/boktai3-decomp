#include "global.h"
struct H { u8 f[8]; u32 v; };
void sub_08030BF8(void);
void sub_08033468(void);
void sub_081BDE10(u8 *p)
{
    ((struct H *)p)[354].v |= 1;
    sub_08030BF8();
    sub_08033468();
}
