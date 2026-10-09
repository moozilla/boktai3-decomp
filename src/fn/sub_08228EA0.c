#include "global.h"
struct G { s32 packed; u8 a,b,c; };
extern struct G gUnk_03005430;
void sub_082286E0(s32 *,s32 *,s32 *,s32);
void sub_0821AAD8(void *);
void sub_0821B4C8(void *,u32,s32);
void sub_08228EA0(void)
{
 s32 a,b,c;
 u32 context[2];
 sub_082286E0(&a,&b,&c,gUnk_03005430.packed);
 sub_0821AAD8(context);
 sub_0821B4C8(context,0,a);
 sub_0821AAD8(context);
 sub_0821B4C8(context,0,b);
 sub_0821AAD8(context);
 sub_0821B4C8(context,0,c);
 sub_0821AAD8(context);
 sub_0821B4C8(context,0,gUnk_03005430.a);
 sub_0821AAD8(context);
 sub_0821B4C8(context,0,gUnk_03005430.b);
 sub_0821AAD8(context);
 sub_0821B4C8(context,0,gUnk_03005430.c);
}
