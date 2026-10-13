#include "global.h"
struct Panel {u8 bytes[0x25c];};struct P {u8 pad[0x2c];struct Panel panels[2];u32 windows[3];u8 pad1[0x144];u8 draw[1];};
void sub_0803746C(struct P*,struct Panel*,int);void sub_08033778(u32);void sub_082195E0(void*);
int sub_08037B6C(struct P*p){struct Panel*q=p->panels;int i=0;u32*w;do{sub_0803746C(p,q,i);i++;q++;}while(i<=1);w=p->windows;i=2;do{sub_08033778(*w++);i--;}while(i>=0);sub_082195E0(p->draw);return 0;}
