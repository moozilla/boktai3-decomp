#include "global.h"
int sub_08019F2C(void *);
void sub_082279A8(int, int, int, int, int, int, int);
void SelectionMenu_Hide(void *);
void sub_08033590(void);
void sub_0801A094(void *);
void Task_Kill(void *);
void SelectionMenu_Confirm(u8 *p)
{
    if (sub_08019F2C(p))
        sub_082279A8(1, 5, 4, 4, 4, 0xffff, 0);
    if (*(u32 *)(p + 0x28) > 31) {
        SelectionMenu_Hide(p);
        sub_08033590();
        if (*(u16 *)(p + 0x42))
            sub_0801A094(p);
        Task_Kill(p);
    } else
        (*(u32 *)(p + 0x28))++;
}
