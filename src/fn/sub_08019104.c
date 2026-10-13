#include "global.h"
void sub_0801901C(void);
void VBlankIntrWait(void);
void sub_08216F78(int, int, int, int, int);
extern void *gUnk_0200009C;
#define CircleWindow_Destroy sub_08019104
int CircleWindow_Destroy(void)
{
    sub_0801901C();
    VBlankIntrWait();
    sub_08216F78(0, 0, 0, 240, 160);
    gUnk_0200009C = 0;
    return 0;
}
