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
void *sub_08219FBC(int, int);
void sub_0821A04C(void *, void *, void *);
int sub_08033EA4(struct Transition *);
int sub_08033FEC(struct Transition *);
int sub_08033E18(struct Transition *);
int sub_08033E74(struct Transition *);
struct Transition *sub_08034030(void) {
  struct Transition *p;
  if (gUnk_020000E8) {
    sub_08033EA4(gUnk_020000E8);
    return gUnk_020000E8;
  }
  p = sub_08219FBC(8, sizeof(*p));
  if (p) {
    sub_0821A04C(p, sub_08033E18, sub_08033E74);
    if (sub_08033FEC(p) < 0) {
      sub_0821A0C0(p);
      return 0;
    }
  }
  return p;
}
