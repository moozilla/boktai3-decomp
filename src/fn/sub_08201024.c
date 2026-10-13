#include "global.h"
struct E {
    u8 p[0x344];
};
struct A {
    u8 p0[0x1c];
    struct E e[3];
};
extern u32 gUnk_020005F8;
void Collider_Remove(void *);
void sub_08013B74(void *);
void sub_08160624(u32);
void Task_Kill(u32);
void Anim_Remove(void *);
u32 sub_08201024(struct A *a)
{
    u8 *e = (u8 *)a + 0x1c;
    s32 i;
    for (i = 2; i >= 0; i--, e += 0x344) {
        u32 *q;
        Collider_Remove(e + 0x80);
        sub_08013B74(e + 0x298);
        q = (u32 *)(e + 0x33c);
        if (*q != 0) {
            sub_08160624(*q);
            Task_Kill(*q);
        }
        Anim_Remove(e + 0x1b8);
    }
    return gUnk_020005F8 = 0;
}
