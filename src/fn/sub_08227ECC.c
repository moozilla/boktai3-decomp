#include "global.h"
extern s32 *gUnk_030053F8;
s32 Div(s32,s32);s32 Mod(s32,s32);
void sub_0821AAD8(void*);void sub_0821B4C8(void*,s32,s32);
void sub_08227ECC(void)
{
 u32 context[2];
 s32 q=Div(*gUnk_030053F8,60);
 s32 r;
 Mod(q,60);
 q=Div(q,60);r=Mod(q,60);q=Div(q,60);
 sub_0821AAD8(context);sub_0821B4C8(context,0,0);
 sub_0821AAD8(context);sub_0821B4C8(context,0,q);
 sub_0821AAD8(context);sub_0821B4C8(context,0,r);
}
