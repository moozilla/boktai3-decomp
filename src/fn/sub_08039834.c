#include "global.h"
struct Panel {u8 pad0[0x60];void*task;};
void sub_0803A4D4(int,int);void*sub_0803AC5C(void);void sub_08037ECC(void);void sub_08037DEC(void);void sub_08037F30(void);void sub_08037F50(void);void sub_08036B2C(void*,void*,void*,void*);void sub_080485E0(int,int);
void sub_08039834(struct Panel*p){sub_0803A4D4(0,0);p->task=sub_0803AC5C();sub_08036B2C(sub_08037ECC,sub_08037DEC,sub_08037F30,sub_08037F50);sub_080485E0(4,24);}
