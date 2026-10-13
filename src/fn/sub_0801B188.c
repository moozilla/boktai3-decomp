#include "global.h"
int sub_08019F2C(void *);
void sub_082279A8(int, int, int, int, int, int, int);
void sub_0821AD08(int, void *);
extern u32 gUnk_0300523C;
void SelectionMenu_Close(u8 *p)
{
    if (sub_08019F2C(p))
        sub_082279A8(3, 4, 4, 4, 4, 0xffff, 0);
    if (*(u32 *)(p + 0x28) == 20) {
        {
            u32 *state = &gUnk_0300523C;
            int mask;
            mask = ~4;
            *state &= mask;
        }
        if (*(u16 *)(p + 0x40))
            sub_0821AD08(*(u16 *)(p + 0x40), 0);
    } else
        (*(u32 *)(p + 0x28))++;
}
