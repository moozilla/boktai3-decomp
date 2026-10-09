#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_08201D38(void);void sub_08202120(void);s32 sub_0820212C(void *,void *);
void *sub_08202608(void *p) {
 void *r=sub_08219FBC(8,0x1e0);
 if(r) {
 sub_0821A04C(r,sub_08201D38,sub_08202120);
 if(sub_0820212C(r,p)<0){sub_0821A0C0(r);return 0;}
 return r;
 }
}
