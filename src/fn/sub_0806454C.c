#include "global.h"
void RoamingObjectB_InitializePosition(u8 *);
void sub_08064438(u8 *);
void sub_08064460(u8 *);
void sub_080644D4(u8 *);
void sub_0815F6F0(u8 *, u8 *, void (*)(void));
void sub_08063F84(void);
void RoamingObjectB_SetState(u8 *, u32);
u32 sub_0806454C(u8 *p, u16 v)
{
    *(u16 *)(p + 0x18) = v;
    RoamingObjectB_InitializePosition(p);
    sub_08064438(p);
    sub_08064460(p);
    sub_080644D4(p);
    sub_0815F6F0(p + 0x124, p + 0xBC, sub_08063F84);
    RoamingObjectB_SetState(p, 0);
    return 0;
}
