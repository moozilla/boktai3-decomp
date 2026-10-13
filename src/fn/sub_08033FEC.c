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
extern u16 gUnk_03004BD8, gUnk_03004FC0, gUnk_03005214, gUnk_030051C4,
    gUnk_03005260[];
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
int sub_0821ABA8(int, int);
int sub_08033EA4(struct Transition *);
int sub_08033FEC(struct Transition *p) {
  gUnk_020000E8 = p;
  p->flags = sub_0821ABA8(102, 0);
  if (p->flags & 1) {
    int previous = gUnk_03004BD8;
    p->savedRegister = previous;
    {
      int value = previous & ~0xE00;
      int bits = 0x100;
      value |= bits;
      gUnk_03004BD8 = value;
    }
  }
  return sub_08033EA4(p);
}
