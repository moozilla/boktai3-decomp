#include "global.h"
void sub_08063CAC(u8 *);
void sub_08063DAC(u8 *);
void sub_08063DD4(u8 *);
void sub_08063E48(u8 *);
void sub_0815F6A8(u8 *, u8 *, void (*)(void));
void sub_080638F8(void);
void sub_080638BC(u8 *, u32);
u32 sub_08063EC0(u8 *p, u16 v)
{
    *(u16 *)(p + 0x18) = v;
    sub_08063CAC(p);
    sub_08063DAC(p);
    sub_08063DD4(p);
    sub_08063E48(p);
    sub_0815F6A8(p + 0x124, p + 0x1c, sub_080638F8);
    sub_080638BC(p, 0);
    return 0;
}
