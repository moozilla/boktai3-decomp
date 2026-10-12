// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern u32 gUnk_03005308;extern u16 gUnk_0203B400[];void sub_08029A78(void*,int,int);
int sub_08249350(int,int);
static inline int randomColumn(u16*table){gUnk_03005308=(gUnk_03005308+1)&1023;return sub_08249350(table[gUnk_03005308],3);}
void sub_08029AA8(u8*p){if((*(s16**)(p+0x110))[2]<0xc00){sub_08029A78(p,0,0);sub_08029A78(p,2,0);sub_08029A78(p,0,1);sub_08029A78(p,1,1);sub_08029A78(p,2,1);{register u16*table asm("r6")=gUnk_0203B400;asm(""::"r"(table));sub_08029A78(p,randomColumn(table),2);sub_08029A78(p,randomColumn(table),3);sub_08029A78(p,randomColumn(table),4);}}else{sub_08029A78(p,0,1);sub_08029A78(p,1,1);sub_08029A78(p,2,1);{register u16*table asm("r6")=gUnk_0203B400;asm(""::"r"(table));sub_08029A78(p,randomColumn(table),2);sub_08029A78(p,randomColumn(table),3);sub_08029A78(p,randomColumn(table),4);}}}
