#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_082074F0(void);void sub_082079F4(void);s32 sub_08207A00(void *,void *);
void *sub_08207F68(void *p) {
 void *r=sub_08219FBC(8,0x1a8);
 if(r) {
 sub_0821A04C(r,sub_082074F0,sub_082079F4);
 if(sub_08207A00(r,p)<0){sub_0821A0C0(r);return 0;}
 return r;
 }
}
