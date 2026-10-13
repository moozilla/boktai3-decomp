#include "global.h"
struct P {
    u8 pad[0x3c];
    u32 w3c;
    u8 pad1[0x77];
    u8 bb7;
};
extern u32 gUnk_02000580;
s32 sub_0801B54C(void);
u8 ResultMenu_ConfigureLayers(u32, u32);
void sub_0801BD44(struct P *);
void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void sub_0815BE5C(u32, u32);
void sub_0801B534(struct P *, void (*)(void));
void ResultMenu_Countdown(void);
void sub_0801C5E4(struct P *p)
{
    if (sub_0801B54C() != 0) {
        u32 z;
        p->bb7 = ResultMenu_ConfigureLayers(0xe, 0);
        z = 0;
        sub_0801BD44(p);
        sub_082279A8(0, 5, 4, 4, 4, 0xFFFF, z);
        sub_0815BE5C(gUnk_02000580, 0);
    }
    if (p->w3c > 0x1f) {
        sub_0801B534(p, ResultMenu_Countdown);
    } else {
        p->w3c++;
    }
}
