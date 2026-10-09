#include "global.h"
struct Context { u8 pad[0x4f0]; u32 date; s32 hour,minute,second; };
extern struct Context *gUnk_02000710;
u32 sub_08228D7C(void);s32 sub_08228D88(void),sub_08228D94(void),sub_08228DA0(void);
void sub_082286E0(s32*,s32*,s32*,s32);
u32 sub_082285C4(s32,s32,s32,s32,s32,s32);
u32 sub_0822C404(void) {
 s32 delta;
 s32 a,b,c,d,e,f;
 s32 hour,minute,second,time,old;
 if(sub_08228D7C()==gUnk_02000710->date) delta=0;
 else {
 sub_082286E0(&a,&b,&c,sub_08228D7C());
 sub_082286E0(&d,&e,&f,gUnk_02000710->date);
 if(sub_082285C4(a,b,c,d,e,f)>1) goto yes;
 delta=86400;
 }
 hour=sub_08228D88();minute=sub_08228D94();second=sub_08228DA0();
 time=(hour*60+minute)*60+second;
 old=(gUnk_02000710->hour*60+gUnk_02000710->minute)*60+gUnk_02000710->second;
 delta+=time-old;
 if(delta>179) goto yes;
 return 0;
yes:
 return 1;
}
