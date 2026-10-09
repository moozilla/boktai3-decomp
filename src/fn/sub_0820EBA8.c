#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_0820E2A4(void);void sub_0820E718(void);s32 sub_0820E724(void *,void *);
void *sub_0820EBA8(void *p) {
 void *r=sub_08219FBC(8,0x17c);
 if(r) {
 sub_0821A04C(r,sub_0820E2A4,sub_0820E718);
 if(sub_0820E724(r,p)<0){sub_0821A0C0(r);return 0;}
 return r;
 }
}
