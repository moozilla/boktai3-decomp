#include "global.h"
#define SelectionMenu_OpenActorPage sub_0801ABEC
void sub_0801AB1C(void *);
void sub_082156B8(int);
void sub_0801A10C(int, int);
void sub_0801A4B4(void *);
void sub_0801A53C(void *);
void sub_0801A75C(void *);
void sub_08030C84(u32);
void sub_08030F20(int, int, int, int);
void sub_08030B78(int);
void sub_080311B0(int);
void SelectionMenu_OpenActorPage(u8 *p)
{
    sub_0801AB1C(p);
    p[0x1e] = 0;
    p[0x24] = 0;
    sub_082156B8(0);
    sub_0801A10C(13, 0);
    sub_0801A4B4(p);
    sub_0801A53C(p);
    sub_0801A75C(p);
    sub_08030C84(*(u32 *)(p + 0xda8));
    sub_08030F20(2, 16, 30, 2);
    sub_08030B78(1);
    sub_080311B0(0x81b);
}
