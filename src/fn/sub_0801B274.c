#include "global.h"
#define SelectionMenu_Confirm sub_0801B274
int sub_08019F2C(void *);
void sub_082279A8(int, int, int, int, int, int, int);
void sub_0801B1E8(void *);
void sub_08033590(void);
void sub_0801A094(void *);
void sub_0821A0C0(void *);
void SelectionMenu_Confirm(u8 *p)
{
    if (sub_08019F2C(p))
        sub_082279A8(1, 5, 4, 4, 4, 0xffff, 0);
    if (*(u32 *)(p + 0x28) > 31) {
        sub_0801B1E8(p);
        sub_08033590();
        if (*(u16 *)(p + 0x42))
            sub_0801A094(p);
        sub_0821A0C0(p);
    } else
        (*(u32 *)(p + 0x28))++;
}
