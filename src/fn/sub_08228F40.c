#include "global.h"
s32 Script_GetValue(void);
s32 sub_082285C4(s32,s32,s32,s32,s32,s32);
void sub_0821AAD8(void *);
void sub_0821B4C8(void *,u32,s32);
void sub_08228F40(void)
{
 s32 a,b,c,d,e,f,result;
 u32 context[2];
 a=Script_GetValue(); b=Script_GetValue(); c=Script_GetValue();
 d=Script_GetValue(); e=Script_GetValue(); f=Script_GetValue();
 result=sub_082285C4(a,b,c,d,e,f);
 sub_0821AAD8(context);
 sub_0821B4C8(context,0,result);
}
