#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 Time_LoadRegionList(void *);
void sub_0821A0C0(void *);
void Time_HandleRegionKeypad(void);
void Time_DestroyRegionMenu(void);
void *sub_081DCDF4(void)
{
    void *p = sub_08219FBC(0xb, 0x5b8);
    if (p) {
        sub_0821A04C(p, Time_HandleRegionKeypad, Time_DestroyRegionMenu);
        if (Time_LoadRegionList(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
