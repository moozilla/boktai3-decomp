#include "global.h"
struct Task  {
  u8 pad0[0x18];
  void*listener;
  u16 state;
}
;
void*sub_0821ABA8(int,int);
void sub_0821447C(int,void*,void*,void*);
void sub_08218B30(void);
void sub_08215114(void);
void sub_082199C4(void);
int sub_08039568(struct Task*p) {
  p->listener=sub_0821ABA8(101,0);
  p->state=0;
  sub_0821447C(0,sub_08218B30,sub_08215114,sub_082199C4);
  return 0;
}
