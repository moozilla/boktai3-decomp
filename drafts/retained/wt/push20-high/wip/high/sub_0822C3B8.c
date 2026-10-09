#include "global.h"
struct Context { u8 pad[0x4f0]; u32 date; s32 hour,minute,second; };
extern struct Context *gUnk_02000710;
u32 sub_08228D7C(void);s32 sub_08228D88(void),sub_08228D94(void),sub_08228DA0(void);
void sub_0822C3B8(void) {
 gUnk_02000710->date=sub_08228D7C();
 gUnk_02000710->hour=sub_08228D88();
 gUnk_02000710->minute=sub_08228D94();
 gUnk_02000710->second=sub_08228DA0();
}
