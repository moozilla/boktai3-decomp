#include "global.h"
// MATCHING_CONCESSION: register-pinning
extern int gUnk_02000080,gUnk_02000084;extern u32 gUnk_03005308;extern u16 gUnk_0203B400[];int Script_SeekToKeyword(int);int Script_GetValue(void);int Mod(int,int);
#define Weather_ConfigureCooldown sub_080183A8
int Weather_ConfigureCooldown(void){int base,spread,result;if(Script_SeekToKeyword('c')){base=Script_GetValue();spread=Script_GetValue();}else{base=spread=1200;}result=Script_SeekToKeyword('i');if(result)result=Script_GetValue();else if(gUnk_02000084>0){gUnk_03005308=(gUnk_03005308+1)&1023;{u32 offset=gUnk_03005308*2;asm("" : "+r"(offset));result=Mod(*(u16*)((u8*)gUnk_0203B400+offset),gUnk_02000084);}}gUnk_02000080=base;gUnk_02000084=spread;}
