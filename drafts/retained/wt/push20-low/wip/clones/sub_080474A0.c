#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_08047338(void);void sub_080473F0(void);s32 sub_08047424(void *,void *);
void *sub_080474A0(void *p) {
 void *r=sub_08219FBC(10,0x160);
 if(r) {
 sub_0821A04C(r,sub_08047338,sub_080473F0);
 if(sub_08047424(r,p)<0){sub_0821A0C0(r);return 0;}
 return r;
 }
}
