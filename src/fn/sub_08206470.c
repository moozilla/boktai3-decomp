#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_08206030(void);void sub_0820608C(void);s32 sub_08206098(void *,void *);
void *sub_08206470(void *p) {
 void *r=sub_08219FBC(8,0x160);
 if(r) {
 sub_0821A04C(r,sub_08206030,sub_0820608C);
 if(sub_08206098(r,p)<0){sub_0821A0C0(r);return 0;}
 return r;
 }
}
