#include "global.h"
void sub_08071434(void);
void Heap_Free(u32);
void sub_08073C04(void);
extern u32 gUnk_020004AC;
extern u32 gUnk_020004A8;
u32 sub_08071D3C(void)
{
    sub_08071434();
    Heap_Free(gUnk_020004AC);
    sub_08073C04();
    gUnk_020004A8 = 0;
}
