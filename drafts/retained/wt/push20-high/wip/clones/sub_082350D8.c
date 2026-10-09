#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_08234ECC(void);void sub_08234EF4(void);s32 sub_08235000(void *,void *);
void *sub_082350D8(void *p) {
 void *r=sub_08219FBC(11,0x160);
 if(r) {
 sub_0821A04C(r,sub_08234ECC,sub_08234EF4);
 if(sub_08235000(r,p)<0){sub_0821A0C0(r);return 0;}
 return r;
 }
}
