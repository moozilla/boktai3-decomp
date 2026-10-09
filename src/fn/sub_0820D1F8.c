#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_0820C9AC(void);void sub_0820CCF4(void);s32 sub_0820CD00(void *,void *);
void *sub_0820D1F8(void *p) {
 void *r=sub_08219FBC(8,0x220);
 if(r) {
 sub_0821A04C(r,sub_0820C9AC,sub_0820CCF4);
 if(sub_0820CD00(r,p)<0){sub_0821A0C0(r);return 0;}
 return r;
 }
}
