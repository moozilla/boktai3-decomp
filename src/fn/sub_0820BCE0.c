#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_0820B3E0(void);void sub_0820B7D4(void);s32 sub_0820B7E0(void *,void *);
void *sub_0820BCE0(void *p) {
 void *r=sub_08219FBC(8,0x198);
 if(r) {
 sub_0821A04C(r,sub_0820B3E0,sub_0820B7D4);
 if(sub_0820B7E0(r,p)<0){sub_0821A0C0(r);return 0;}
 return r;
 }
}
