#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void *,void *);void sub_0821A0C0(void *);
void sub_08203DF4(void);void sub_08204224(void);s32 sub_08204230(void *,void *);
void *sub_08204704(void *p) {
 void *r=sub_08219FBC(8,0x198);
 if(r) {
 sub_0821A04C(r,sub_08203DF4,sub_08204224);
 if(sub_08204230(r,p)<0){sub_0821A0C0(r);return 0;}
 return r;
 }
}
