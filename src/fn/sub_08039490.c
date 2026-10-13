#include "global.h"
struct Task {u8 pad0[0x16];u8 active;};
void*sub_08219FBC(int,int);void sub_0821A04C(void*,void*,void*);int sub_08039058(void*);int sub_080390A0(void*);int sub_080390C4(void*);void sub_0821A0C0(void*);
struct Task*sub_08039490(void){struct Task*p=sub_08219FBC(2,276);if(p){sub_0821A04C(p,sub_08039058,sub_080390A0);if(sub_080390C4(p)<0){sub_0821A0C0(p);return 0;}p->active=1;}return p;}
