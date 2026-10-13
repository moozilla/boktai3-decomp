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
extern void *gUnk_086056D8[];
void sub_0824923C(struct Transition *, void *);
int sub_08033E18(struct Transition *p) {
  void *callback = gUnk_086056D8[p->state];
  if (callback)
    sub_0824923C(p, callback);
  {
    u16 *reg = &gUnk_03004FC0;
    int value = 0x1084;
    *reg = value;
  }
  if ((gUnk_03005260[1] & 9) && p->endProcedure && p->state <= 2)
    sub_08033C90(p, 3);
  p->ticks++;
  return 0;
}
