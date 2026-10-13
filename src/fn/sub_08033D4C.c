#include "global.h"
struct Transition {
    u8 pad0[0x18];
    u8 state, entered, index, lastIndex;
    u32 parameter, ticks;
    u16 flags, savedRegister;
    void *script;
    void *entries;
    u16 startProcedure, endProcedure;
    s16 delays[64];
    u16 fadeIn[64], fadeOut[64], x[64], y[64], width[64], height[64];
};
extern struct Transition *gUnk_020000E8;
extern u32 gUnk_03004DA0;
extern u16 gUnk_03004BD8, gUnk_03004FC0, gUnk_03005214, gUnk_030051C4, gUnk_03005260[];
void sub_08030F20(int, int, int, int);
void sub_08030DBC(int);
void sub_08030BF8(void);
void sub_08030B78(int);
void sub_08030C84(void *);
void sub_080311B0(int);
void sub_0821AD08(int, int);
void sub_0821A0C0(void *);
void sub_082156B8(int);
int Div(int, int);
u8 sub_08033C9C(struct Transition *);
void sub_08033C90(struct Transition *, int);
void sub_08033C58(struct Transition *);
void sub_08033D4C(struct Transition *p)
{
    sub_08033C9C(p);
    if (p->ticks >= p->fadeOut[p->index]) {
        gUnk_03004DA0 = 0;
        if (p->index == p->lastIndex) {
            sub_08030BF8();
            sub_08033C90(p, 4);
            if (p->startProcedure)
                sub_0821AD08(p->startProcedure, 0);
            if (p->flags & 2)
                sub_0821A0C0(p);
        } else {
            p->index++;
            sub_08033C90(p, 0);
        }
    } else
        gUnk_03004DA0 = 64 - Div(p->ticks << 6, p->fadeOut[p->index]);
}
