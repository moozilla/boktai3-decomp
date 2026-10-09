#include "global.h"
u32 sub_0822BB4C(void),sub_0822ED64(u32),sub_0822BAF4(u32);
void *sub_0822BB28(void);
s32 sub_0822F0A4(u32,void*,u32);
u32 sub_0822BCE4(u32 arg) {
 u32 size=sub_0822ED64(sub_0822BB4C());
 u32 index=sub_0822BAF4(arg);
 s32 result;
 index<<=16;index>>=16;
 result=sub_0822F0A4(index,sub_0822BB28(),size);
 if(result!=size+8) return 0;
 return 1;
}
