#include "global.h"
#define ResultGame_Finish sub_0801CEF0
int sub_0801B54C(void*);void sub_0801C1E8(void*,int,int,void*);void sub_0822AE7C(int);void sub_0801B534(void*,void*);void sub_0801BA0C(void*);
void ResultGame_Finish(u8*p){if(sub_0801B54C(p)){sub_0801C1E8(p,0x37,0,0);sub_0822AE7C(0xa9);}if(*(u32*)(p+0x3c)>29)sub_0801B534(p,sub_0801BA0C);else ++*(u32*)(p+0x3c);}
