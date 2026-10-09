#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_08208740(void);void sub_08208BD8(void);s32 sub_08208BE4(void *,void *);
void *sub_08208FD8(void *p) {
 void *r=sub_08219FBC(8,0x168);
 if(r) {
 sub_0821A04C(r,sub_08208740,sub_08208BD8);
 if(sub_08208BE4(r,p)<0){sub_0821A0C0(r);return 0;}
 return r;
 }
}
