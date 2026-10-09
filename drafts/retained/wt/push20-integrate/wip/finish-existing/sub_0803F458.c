#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_0803F3A4(void);void sub_0803F3D8(void);s32 sub_0803F40C(void *);
void *sub_0803F458(void) {void *r=sub_08219FBC(10,0x9a0);
if(r) {sub_0821A04C(r,sub_0803F3A4,sub_0803F3D8);if(sub_0803F40C(r)<0){sub_0821A0C0(r);return 0;}
}
return r;
}
