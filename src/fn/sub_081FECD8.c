#include "global.h"
struct S {
    u8 p[0x14];
    u16 h14;
    u8 q[10];
};
void sub_081FE92C(struct S *);
void SunOrb_SetMotionParams(struct S *, u32, u32, u32, u32);
void SunOrb_SetTargetSource(struct S *, u32);
u32 sub_081FE760(struct S *, void *);
u32 SunOrb_CreateSceneTargeted(void *a)
{
    struct S s;
    sub_081FE92C(&s);
    SunOrb_SetMotionParams(&s, 0, 0, 0, 0);
    SunOrb_SetTargetSource(&s, 1);
    s.h14 = 1;
    return sub_081FE760(&s, a);
}
