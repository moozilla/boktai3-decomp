#include "global.h"
void sub_08030BF8(void);
void sub_08033468(void);
void sub_08033590(void);
void sub_0816D920(void *, u32);
s32 sub_0816CEB4(u32, u32);
s32 sub_0822D604(s32);
void sub_08165E78(u8 *, u32, s32, u32, void *, void *, u32, u32);
void sub_0822B2F8(u32);
void sub_08163EB8(u8 *, void (*)(void), u32);
void sub_0816F768(void);
struct E
{
  u32 flags;
  u8 pad[0x2E];
  u16 v;
  u8 tail[0x2C];
};
struct S
{
  u8 pad[0x1428];
  struct E elems[266];
};
void sub_0816F6B4(u8 *s)
{
  u32 v[2];
  u32 w[2];
  u8 *kind;
  u32 index;
  u32 later;
  u32 *flags;
  s32 result;
  sub_08030BF8();
  sub_08033468();
 do { sub_08033590(); kind = s + 0x438A; index = (*kind) + 10; } while (0);
  flags = (u32 *)((u32)s + index * sizeof(struct E) + 0x1428);
  *flags |= 1;
  sub_0816D920(v, *kind);
  sub_0816D920(w, s[0x4387]);
  result = sub_0822D604(sub_0816CEB4(s[0x4860], *kind));
  later = (*kind) + 10;
  sub_08165E78(s, 0, result, ((struct S *) s)->elems[later].v, v, w, 0x18, 0x10);
  sub_0822B2F8(0x38A);
  sub_08163EB8(s, sub_0816F768, 1);
}
