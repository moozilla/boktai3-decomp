#include "global.h"
void *sub_08219FBC(s32,s32);void sub_0821A04C(void *,void(*)(void),void(*)(void));void sub_0821A0C0(void *);
void sub_081B5D8C(void);void sub_081B5E38(void);s32 sub_081B5EA4(void *,s32,s32,s32);extern void *gUnk_02000254;
void *sub_081B6180(s32 a,s32 b,s32 c){
 void **global=&gUnk_02000254;void *p=sub_08219FBC(9,0x5b8);
 if(p){sub_0821A04C(p,sub_081B5D8C,sub_081B5E38);*global=p;if(sub_081B5EA4(p,a,b,c)<0){sub_0821A0C0(p);return 0;}}
 return p;
}
