#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_08045C20(void);void sub_08045C38(void);s32 sub_08045C44(void *,void *);
void *sub_08045CB0(void *p) {
 void *r=sub_08219FBC(10,0x160);
 if(r) {
 sub_0821A04C(r,sub_08045C20,sub_08045C38);
 if(sub_08045C44(r,p)<0){sub_0821A0C0(r);return 0;}
 return r;
 }
}
