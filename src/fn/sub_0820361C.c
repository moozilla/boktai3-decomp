#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_08203204(void);void sub_08203370(void);s32 sub_0820337C(void *,void *);
void *sub_0820361C(void *p) {
 void *r=sub_08219FBC(8,0x104);
 if(r) {
 sub_0821A04C(r,sub_08203204,sub_08203370);
 if(sub_0820337C(r,p)<0){sub_0821A0C0(r);return 0;}
 return r;
 }
}
