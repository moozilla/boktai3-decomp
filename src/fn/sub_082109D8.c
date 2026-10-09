#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_08210398(void);void sub_08210648(void);s32 sub_08210654(void *,void *);
void *sub_082109D8(void *p) {
 void *r=sub_08219FBC(8,0x160);
 if(r) {
 sub_0821A04C(r,sub_08210398,sub_08210648);
 if(sub_08210654(r,p)<0){sub_0821A0C0(r);return 0;}
 return r;
 }
}
