#include "global.h"
u32 sub_08228DB8(void);
void sub_08228D88(void);
void sub_08228D94(void);
void sub_0816589C(u32, u32);
void sub_08165830(u32, u32);
void sub_08165870(u32, u32);
void sub_081657F0(u32, u32);
void sub_081755FC(void)
{
    sub_08228D88();
    sub_08228D94();
    switch (sub_08228DB8()) {
    case 0: case 4: case 5:
        sub_0816589C(0x13, 0xb);
        sub_08165830(0x18, 0xb);
        break;
    case 1: case 2: case 3:
        sub_08165870(0x13, 0xb);
        sub_081657F0(0x18, 0xb);
        break;
    }
}
