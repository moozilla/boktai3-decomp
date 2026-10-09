#include "global.h"
struct Packed {u32 a:16,b:16,c:16,pad:16;};
extern u32 gUnk_03005420;
u32 Script_SeekToKeyword(u32);s32 Script_GetValue(void);
void sub_082265F8(u32);void sub_0822632C(s32,struct Packed*);
void sub_082266D4(void)
{
 struct Packed pos;
 if(Script_SeekToKeyword('f')) {
  s32 value=Script_GetValue();
  if(Script_SeekToKeyword('p')) {
   pos.a=Script_GetValue();pos.b=Script_GetValue();pos.c=Script_GetValue();
   sub_082265F8(gUnk_03005420);sub_0822632C(value,&pos);
  }
 }
}
