#include "global.h"
struct Task  {
  u8 pad0[0x18];
  void*listener;
  u16 state;
}
;
void sub_082279A8(int,int,int,int,int,int,int);
void sub_08036C54(void);
void*sub_08037DC0(void);
void sub_0821A0C0(void*);
int sub_08034084(void);
void sub_0821AD08(void*,int);
int sub_080394D4(struct Task*p) {
  switch(p->state) {
    case 0:sub_082279A8(4,0,0,0,0,65535,0);
    sub_08036C54();
    {
      void*q=sub_08037DC0();
      if(q)sub_0821A0C0(q);
    }
    p->state++;
    break;
    case 1:if(!sub_08034084())p->state++;
    break;
    case 2:if(p->listener)sub_0821AD08(p->listener,0);
    sub_0821A0C0(p);
    break;
  }
  return 0;
}
