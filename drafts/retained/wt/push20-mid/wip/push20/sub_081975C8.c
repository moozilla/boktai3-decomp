#include "global.h"
u32 sub_0821ABA8(u32,u32);
u32 Script_SeekToKeyword(u32);
u32 Script_GetValue(void);
void sub_081975C8(u8 *s)
{
    u32 value=sub_0821ABA8(0x6C,0xFA0);
    u8 bytezero;
    u16 halfzero;
    u16 *first,*second,*third;
    u8 *flag;
    *(u16 *)(s+0x7268)=(bytezero=0,halfzero=0,value);
    *(u16 *)(s+0x7276)=value;
    *(u16 *)(s+0x726A)=sub_0821ABA8(0x67,0x64);
    *(u16 *)(s+0x726C)=sub_0821ABA8(0x59,0x3E8);
    *(u32 *)(s+0x72C0)=sub_0821ABA8(0x65,0);
    *(u16 *)(s+0x726E)=sub_0821ABA8(0x41,0x28);
    *(u16 *)(s+0x7270)=sub_0821ABA8(0x44,0x14);
    *(u16 *)(s+0x7272)=sub_0821ABA8(0x45,0x64);
    *(u16 *)(s+0x7274)=sub_0821ABA8(0x53,0);
    first=(u16 *)(s+0x72A0);
    *first=halfzero;
    second=(u16 *)(s+0x72A2);
    *second=halfzero;
    third=(u16 *)(s+0x72A4);
    *third=halfzero;
    flag=s+0x72A6;
    *flag=bytezero;
    if(Script_SeekToKeyword(0x63)) {
        *first=Script_GetValue();
        *second=Script_GetValue();
        *flag=bytezero;
    }
    if(Script_SeekToKeyword(0x66)) {
        *first=Script_GetValue();
        *second=Script_GetValue();
        *third=Script_GetValue();
        *flag=1;
    }
}
