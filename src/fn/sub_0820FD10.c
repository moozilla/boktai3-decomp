#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_0820F378(void);void sub_0820F824(void);s32 sub_0820F830(void *,void *);
void *sub_0820FD10(void *p) {
 void *r=sub_08219FBC(8,0x198);
 if(r) {
 sub_0821A04C(r,sub_0820F378,sub_0820F824);
 if(sub_0820F830(r,p)<0){sub_0821A0C0(r);return 0;}
 return r;
 }
}
