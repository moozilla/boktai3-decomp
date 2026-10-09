#include "global.h"
struct S { u8 p0[12]; s16 x,y,z; };
s32 Script_GetValue(void);
struct S *sub_082258DC(u16);
void sub_0821AAD8(void *);
void sub_0821B4C8(void *,u32,s32);
s32 sub_082259A0(void)
{
 u32 context[2];
 s32 result;
 struct S *s=sub_082258DC((u16)Script_GetValue());
 if(s) {
  sub_0821AAD8(context); sub_0821B4C8(context,0,s->x);
  sub_0821AAD8(context); sub_0821B4C8(context,0,s->y);
  sub_0821AAD8(context); sub_0821B4C8(context,0,s->z);
  result=0;
 }
 else {
  sub_0821AAD8(context); sub_0821B4C8(context,0,0);
  sub_0821AAD8(context); sub_0821B4C8(context,0,0);
  sub_0821AAD8(context); sub_0821B4C8(context,0,0);
  result=-1;
 }
 return result;
}
