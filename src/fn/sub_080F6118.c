#include "global.h"
void sub_080F4CC4(u8 *);
void sub_0824923C(u8 *,void (*)(u8 *));
static inline u8 Take(u8 *s) {
 u32 test=1;
 u8 *p=s+0x119;
 u8 value=*p;
 test &=value;
 if(test) {
  u32 zero;
  s32 mask=-2;mask &=value;
  zero=0;
  *p=mask;
  s[0x12E]=zero;
  return 1;
 }
 return 0;
}
void sub_080F6118(u8 *s) {
 if(Take(s)) {
  void (**p)(u8*)=(void(**)(u8*))(s+0x174);
  void (*fn)(u8*)=sub_080F4CC4;
  *p=fn;
 }
 { void (*fn)(u8*)=*(void(**)(u8*))(s+0x174);
 if(fn)sub_0824923C(s,fn); }
}
