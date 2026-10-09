#include "global.h"
void sub_080297FC(u8*,u32,u32,u32);
void sub_0824923C(u8*,void (*)(u8*));
void sub_0802A87C(u8 *s) {
 if(s[0xB1]) {s[0xB1]=0;sub_080297FC(s,2,1,0);}
 { u32 zero;u32 *p=(u32*)(s+0x178);zero=0;*p=zero;s[0x17C]=zero; }
 { u32 mask=12;u32 *flags=(u32*)(s+0x9C);*flags |=mask; }
 sub_0824923C(s,*(void(**)(u8*))(s+0x108));
}
