#include "global.h"
s32 sub_08210A48(s32);
void sub_0803386C(void*,void*);void sub_08033924(void*,s32);void sub_080337FC(void*);void sub_08033468(void);void sub_0803343C(void*);
u32 Script_ParseStringRef(void*);
void *Text_LookupString(u32);
static inline u32 plus(s32 offset,u32 base) { return base+offset; }
u32 sub_08210CB0(u8 *p)
{
 s16 *idx=(s16*)(p+0xb88);
 if(sub_08210A48(*idx)) {
  void **obj=(void**)(p+0xb94);
  u32 v; s32 t;
  sub_0803386C(*obj,*(void**)(p+0xb98));
  sub_08033924(*obj,*idx);
  sub_080337FC(*obj);sub_08033468();
  v=Script_ParseStringRef(*(void**)(p+0xb9c));
  t=*idx;
  t=t+v;
  sub_0803343C(Text_LookupString(t));
 } else {
  void **obj=(void**)(p+0xb94);
  u32 v; s32 t;
  sub_0803386C(*obj,*(void**)(p+0xb98));
  sub_08033924(*obj,27);
  sub_080337FC(*obj);sub_08033468();
  v=Script_ParseStringRef(*(void**)(p+0xba0));
  t=*idx;
  t=t+v;
  sub_0803343C(Text_LookupString(t));
 }
 return 0;
}
