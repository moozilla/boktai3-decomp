#include "global.h"
struct Task { u8 pad[16]; u16 f10; u8 pad12[2]; u8 f14, f15, f16, f17; };
extern struct Task gUnk_03001668;
extern u32 gUnk_03005278, gUnk_0300523C, gUnk_03005250;
extern u16 gUnk_03005240, gUnk_03005274[4];
void sub_08219C20(void);
void sub_08219F50(void);
void sub_0821A20C(void);
void sub_0821A2E4(void);
s32 sub_08219A94(void);
void sub_0821A04C(struct Task *, s32 (*)(void), void (*)(void));
void sub_08219F74(struct Task *);
void sub_08219AAC(void)
{
    struct Task *task;
    sub_08219C20();
    sub_08219F50();
    sub_0821A20C();
    sub_0821A2E4();
    gUnk_03005278 = 123456;
    task = &gUnk_03001668;
    sub_0821A04C(task, sub_08219A94, 0);
    task->f14 = 0;
    task->f16 = 1;
    task->f10 = 0;
    sub_08219F74(task);
    gUnk_0300523C = 0;
    gUnk_03005240 = 1;
    gUnk_03005250 = 0;
    {
        u16 *start = gUnk_03005274;
        u32 value = 0x3FF;
        u16 *p = start;
        p += 3;
        do {
            *p = value;
            p--;
        } while ((s32)p >= (s32)start);
    }
}
